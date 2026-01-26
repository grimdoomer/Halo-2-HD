
#pragma once
#include "Halo2HD.h"

#pragma warning(disable: 4005)  // Disable warning C4005: macro redefinition
typedef LONG NTSTATUS;
#include <ntstatus.h>

// ------------------------------------------------------------------------------
// General kernel definitions:

typedef struct _ANSI_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    CHAR* Buffer;
} ANSI_STRING, *PANSI_STRING;
static_assert(sizeof(ANSI_STRING) == 8, "ANSI_STRING incorrect struct size");

typedef struct _OBJECT_ATTRIBUTES
{
    HANDLE RootDirectory;
    ANSI_STRING* ObjectName;
    ULONG Attributes;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
static_assert(sizeof(OBJECT_ATTRIBUTES) == 0xC, "OBJECT_ATTRIBUTES incorrect struct size");

typedef struct _IO_STATUS_BLOCK
{
    ULONG Status;
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
static_assert(sizeof(IO_STATUS_BLOCK) == 8, "IO_STATUS_BLOCK incorrect struct size");


// ------------------------------------------------------------------------------
// ATA definitions:

// Register addresses:
#define ATA_COMMAND_PORT                0x1F0

#define ATA_ERROR_REGISTER              ATA_COMMAND_PORT + 1
#define ATA_SECTOR_COUNT_REGISTER       ATA_COMMAND_PORT + 2
#define ATA_DEVICE_SELECT_REGISTER      ATA_COMMAND_PORT + 6
#define ATA_COMMAND_REGISTER            ATA_COMMAND_PORT + 7
#define ATA_STATUS_REGISTER             ATA_COMMAND_PORT + 7

// ATA commands:
#define ATA_CMD_IDENTIFY                0xEC

// ATA pass through IOCTL:
#define IOCTL_IDE_PASS_THROUGH          0x004D028

typedef struct _IDREGS
{
    UCHAR bFeatureReg;
    UCHAR bSectorCountReg;
    UCHAR bSectorNumberReg;
    UCHAR bCylLowReg;
    UCHAR bCylHighReg;
    UCHAR bDriveHeadReg;
    UCHAR bCommandReg;
    UCHAR bHostSendsData;
} IDREGS, *PIDREGS;
static_assert(sizeof(IDREGS) == 8, "IDREGS incorrect struct size");

typedef struct _ATA_PASS_THROUGH_EX
{
    IDREGS IdeReg;
    ULONG DataBufferSize;
    void* DataBuffer;
} ATA_PASS_THROUGH_EX, *PATA_PASS_THROUGH_EX;
static_assert(sizeof(ATA_PASS_THROUGH_EX) == 0x10, "ATA_PASS_THROUGH_EX incorrect struct size");


// ------------------------------------------------------------------------------
// Mm definitions:

#define PAGE_SIZE       4096


// Call to resolve imported functions.
void ResolveKernelImports();

// Patches the max PFN value in MmAllocateContiguousMemoryEx to allow physical memory allocations in the upper 64MB memory region.
void PatchkernelPhysicalMemoryLimit();

#define KRNLIMP     __declspec(dllimport)

// ------------------------------------------------------------------------------
// Kernel exports:
extern "C"
{
    KRNLIMP int __cdecl DbgPrint(const char* psFormat, ...);

    KRNLIMP void HalReadWritePCISpace(ULONG BusNumber, ULONG SlotNumber, ULONG RegisterNumber, void* Buffer, ULONG Length, BOOLEAN WritePCISpace);

    KRNLIMP NTSTATUS HalWriteSMBusValue(UCHAR SlaveAddress, UCHAR CommandCode, BOOLEAN WriteWordValue, ULONG DataValue);

    KRNLIMP void KeStallExecutionProcessor(ULONG Microseconds);

    KRNLIMP void RtlInitAnsiString(ANSI_STRING* DestinationString, const char* SourceString);

    KRNLIMP NTSTATUS NtOpenFile(HANDLE* FileHandle, ACCESS_MASK DesiredAccess, OBJECT_ATTRIBUTES* ObjectAttributes, IO_STATUS_BLOCK* IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions);

    KRNLIMP NTSTATUS NtDeviceIoControlFile(HANDLE FileHandle, HANDLE Event, void* ApcRoutine, void* ApcContext, IO_STATUS_BLOCK* IoStatusBlock, ULONG IoControlCode, void* InputBuffer, ULONG InputBufferLength, void* OutputBuffer, ULONG OutputBufferLength);

    KRNLIMP NTSTATUS NtClose(HANDLE Handle);

    KRNLIMP NTSTATUS NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

    KRNLIMP ULONG __fastcall KfRaiseIrql(ULONG NewIrql);

    KRNLIMP void __fastcall KfLowerIrql(ULONG NewIrql);

    KRNLIMP ULONG KeGetCurrentIrql();

    extern BYTE* IdexChannelObject;

    KRNLIMP void HalReturnToFirmware(int Routine);

    KRNLIMP void* MmAllocateContiguousMemoryEx(size_t NumberOfBytes, ULONG_PTR LowestAcceptableAddress, ULONG_PTR HighestAcceptableAddress, ULONG_PTR Alignment, ULONG Protect);

    KRNLIMP void MmFreeContiguousMemory(void* BaseAddress);
};

// ------------------------------------------------------------------------------
// Xtl functions:
extern "C"
{

#define XC_VIDEO_STANDARD_NTSC_M    1
#define XC_VIDEO_STANDARD_NTSC_J    2
#define XC_VIDEO_STANDARD_PAL_I     3

    DWORD XGetVideoStandard();

#define XC_VIDEO_FLAGS_WIDESCREEN   0x00000001
#define XC_VIDEO_FLAGS_HDTV_720p    0x00000002
#define XC_VIDEO_FLAGS_HDTV_1080i   0x00000004
#define XC_VIDEO_FLAGS_HDTV_480p    0x00000008
#define XC_VIDEO_FLAGS_LETTERBOX    0x00000010
#define XC_VIDEO_FLAGS_PAL_60Hz     0x00000040

    DWORD XGetVideoFlags();

};