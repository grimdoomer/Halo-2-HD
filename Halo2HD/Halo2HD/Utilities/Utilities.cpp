#include "Utilities.h"
#include <intrin.h>

#pragma function(memset)

void lstrcpyA(char* dst, char* src)
{
    while (*src != 0)
        (*dst++) = (*src++);
}

extern "C"
{
    void* __cdecl memset(void* _Dst, int _Val, size_t _Size)
    {
        unsigned char* ptr = (unsigned char*)_Dst;
        while (_Size-- > 0)
            *ptr++ = (unsigned char)_Val;

        return _Dst;
    }
}

void Util_InstallHook(void* address, void* detour)
{
    // Setup the patch code buffer.
    //  push    Address
    //  ret
    BYTE abPatchCode[6] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0xC3 };
    *(ULONG*)&abPatchCode[1] = (ULONG)detour;

    // Disable interrupts until we restore the CR value.
    _disable();

    // Disable write protect.
    ULONG cr0 = __readcr0();
    __writecr0(cr0 & ~0x10000);

    // Copy the patch code bytes to memory.
    for (int i = 0; i < sizeof(abPatchCode); i++)
        *((BYTE*)address + i) = abPatchCode[i];

    // Restore CR value and re-enabled interrupts.
    __writecr0(cr0);
    _enable();
}

void Util_WriteDword(void* address, unsigned int value)
{
    // Disable interrupts until we restore the CR value.
    _disable();

    // Disable write protect.
    ULONG cr0 = __readcr0();
    __writecr0(cr0 & ~0x10000);

    // Write patch value to memory.
    *(unsigned int*)address = value;

    // Restore CR value and re-enabled interrupts.
    __writecr0(cr0);
    _enable();
}

ULONG Util_GetMemoryCapacity()
{
    ULONG memoryTop = 0;

    // Query the host bridge for memory capacity value (CPU_MEMTOP_LIMIT).
    HalReadWritePCISpace(0, 0, 0x87, &memoryTop, 1, FALSE);

    // Convert the memory top limit to number of MB present.
    return (memoryTop + 1) * 16;
}

void Util_SetFanSpeed(int percent)
{
    // Map the fan speed percent from [0, 100] -> [0, 50].
    percent = percent >> 1;

    // Set the custom fan speed value.
    HalWriteSMBusValue(
        0x20,       // SMC_SLAVE_ADDRESS
        6,          // SMC_COMMAND_REQUEST_FAN_SPEED
        FALSE,
        percent);

    // Give the SMC a chance to process the message, if not it can panic.
    KeStallExecutionProcessor(100 * 1000);      // 100ms

    // Set SMC fan mode to use custom speed.
    HalWriteSMBusValue(
        0x20,       // SMC_SLAVE_ADDRESS
        5,          // SMC_COMMAND_FAN_OVERRIDE
        FALSE,
        1           // SMC_FAN_OVERRIDE_USE_REQUESTED_FAN_SPEED
    );
}

void Util_OverclockGPU(int step)
{
    // Get the current NVPLL_COEFF value.
    volatile ULONG* NVPLL_COEFF = (volatile ULONG*)0xFD680500;
    ULONG coeff = *NVPLL_COEFF;

    /*
        NVPLL_COEFF (32 bits):
            Bits 0-7: M
            Bits 8-15: N
            Bits 16-18: P

        BASE_CLK = 16.6667 Mhz

        nvclk = (N * BASE_CLK / (1 << P) / M)
    */
    coeff = (1 << 16) | (step & 0xFF) << 8 | 1;

    // Mask out the old NDIV value.
    //coeff &= ~0xFF00;

    // Mask in the new NDIV value from the config file.
    //coeff |= (step & 0xFF) << 8;

    // Write the new NVPLL_COEFF value.
    *NVPLL_COEFF = coeff;
}

bool Util_AtaIdentify(BYTE* pIdentifyData)
{
    bool result = false;
    ANSI_STRING DevicePath;
    HANDLE hHdd = NULL;
    IO_STATUS_BLOCK IoStatus = { 0 };
    ATA_PASS_THROUGH_EX AtaReq = { 0 };

    // Zero-init the identify buffer.
    memset(pIdentifyData, 0, 512);

    // Initialize the device path.
    RtlInitAnsiString(&DevicePath, "\\Device\\Harddisk0\\Partition0");

    // Setup the object attributes.
    OBJECT_ATTRIBUTES ObjAttr = { NULL, &DevicePath, 0 };

    // Open the hdd for raw device access.
    NTSTATUS status = NtOpenFile(&hHdd, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, &ObjAttr, &IoStatus, FILE_SHARE_READ | FILE_SHARE_WRITE, 0);
    if (status != STATUS_SUCCESS)
        goto Cleanup;

    // Setup the ATA request.
    AtaReq.IdeReg.bCommandReg = ATA_CMD_IDENTIFY;
    AtaReq.DataBuffer = pIdentifyData;
    AtaReq.DataBufferSize = 512;

    // Submit the ATA passthrough request.
    status = NtDeviceIoControlFile(hHdd, nullptr, nullptr, nullptr, &IoStatus, IOCTL_IDE_PASS_THROUGH, &AtaReq, sizeof(AtaReq), &AtaReq, sizeof(AtaReq));
    if (status != STATUS_SUCCESS)
    {
        // Check if the operation is pending or not.
        if (status != STATUS_PENDING)
            goto Cleanup;

        // Wait for the operation to complete.
        status = NtWaitForSingleObject(hHdd, FALSE, nullptr);
        if (status != STATUS_SUCCESS)
            goto Cleanup;

        // Check the operation result.
        if (IoStatus.Status != STATUS_SUCCESS)
            goto Cleanup;
    }

    // Operation completed successfully.
    result = true;

Cleanup:

    // Close the file handle if the hdd was opened.
    if (hHdd != NULL)
        NtClose(hHdd);

    return result;
}

bool Util_WaitForHddReady(int* pStatus)
{
    // Wait up to 1 second for the HDD to become ready.
    for (int i = 0; i < 10000; i++)
    {
        // Check if the hdd is still busy.
        *pStatus = __inbyte(ATA_STATUS_REGISTER);
        if ((*pStatus & 0x80) == 0)
            return true;

        // Stall for a bit.
        KeStallExecutionProcessor(100);
    }

    // If we made it here the hdd didn't ready up in time.
    return false;
}

bool Util_HddSetTransferSpeed(int mode)
{
    BYTE abIdentifyData[512] = { 0 };
    int udmaMode = 7;
    int deviceStatus = 0;

    // Get the ATA_IDENTIFY data from the device.
    bool result = Util_AtaIdentify(abIdentifyData);
    if (result == false)
        return false;

    // Get the highest UDMA mode supported.
    ULONG ultraDMASupport = *(ULONG*)&abIdentifyData[0xB0];
    for (udmaMode = 7; udmaMode >= 0; udmaMode--)
    {
        if ((ultraDMASupport & (1 << udmaMode)) != 0)
            break;
    }

    // Clamp the UDMA mode to [2, mode] to ensure we don't exceed the mode specified by the config file
    // even if the drive can run faster.
    udmaMode = clamp(udmaMode, 2, mode);
    DbgPrint("Util_HddSetTransferSpeed: mode=%d (%d)\n", udmaMode, mode);

    // Raise IRQL level to surpress any interrupts that might occur while changing device speed.
    BYTE ideIrql = IdexChannelObject[0x18];
    ULONG oldIrql = KfRaiseIrql(ideIrql);

    // Select the HDD device.
    __outbyte(ATA_DEVICE_SELECT_REGISTER, 0xA0);

    // Wait until the device is ready.
    if (Util_WaitForHddReady(&deviceStatus) == false)
        goto Cleanup;

    // Set the transfer mode and speed.
    __outbyte(ATA_ERROR_REGISTER, 3);
    __outbyte(ATA_SECTOR_COUNT_REGISTER, 0x40 + udmaMode);
    __outbyte(ATA_COMMAND_REGISTER, 0xEF);

    // Wait until the HDD is ready.
    if (Util_WaitForHddReady(&deviceStatus) == false)
        goto Cleanup;

    // Check HDD status for errors.
    if ((deviceStatus & 1) != 0)
        goto Cleanup;

    // HDD transfer speed updated successfully.
    result = true;

Cleanup:

    // Restore old IRQL level.
    KfLowerIrql(oldIrql);

    return result;
}