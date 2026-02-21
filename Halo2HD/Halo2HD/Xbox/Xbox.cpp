
#include "Xbox.h"
#include "Utilities/Utilities.h"

extern "C"
{
    extern ULONG* __IMPORT_TABLE__;
}

struct EXPORT_DIRECTORY_TABLE
{
    /* 0x00 */ ULONG ExportFlags;
    /* 0x04 */ ULONG TimeStamp;
    /* 0x08 */ WORD MajorVersion;
    /* 0x0A */ WORD MinorVersion;
    /* 0x0C */ ULONG NameRVA;
    /* 0x10 */ ULONG OrdinalBase;
    /* 0x14 */ ULONG AddressTableEntries;
    /* 0x18 */ ULONG NumberOfNamePointers;
    /* 0x1C */ ULONG ExportAddressTableRVA;
    /* 0x20 */ ULONG NamePointerRVA;
    /* 0x24 */ ULONG OrdinalTableRVA;
};

void UnresolvedImportStub()
{
    _asm
    {
        int 3
    }
}

void ResolveKernelImports()
{
    // Get the kernel export directory from the kernel image header.
    ULONG optionalHeaderOffset = *(ULONG*)(0x80010000 + 0x3C);
    ULONG exportDirectoryOffset = *(ULONG*)(0x80010000 + optionalHeaderOffset + 0x78);

    EXPORT_DIRECTORY_TABLE* pExportDirectory = (EXPORT_DIRECTORY_TABLE*)(0x80010000 + exportDirectoryOffset);
    ULONG* pExportTable = (ULONG*)(0x80010000 + pExportDirectory->ExportAddressTableRVA);

    // Resolve imports.
    ULONG* pImportTable = __IMPORT_TABLE__;
    while (*pImportTable != 0)
    {
        // Make sure the ordinal is valid.
        ULONG ordinal = *pImportTable;
        if (ordinal - pExportDirectory->OrdinalBase >= pExportDirectory->AddressTableEntries)
        {
            *pImportTable = (ULONG)UnresolvedImportStub;
            continue;
        }

        // Resolve function address.
        *pImportTable = 0x80010000 + pExportTable[ordinal - pExportDirectory->OrdinalBase];
        pImportTable++;
    }
}

void PatchkernelPhysicalMemoryLimit()
{
    // Loop scan MmAllocateContiguousMemoryEx for the instruction that holds the max PFN value.
    BYTE* pCodePtr = (BYTE*)MmAllocateContiguousMemoryEx;
    for (int i = 0; i < 0x80; i++)
    {
        // Check for the following instruction that holds the max PFN to allocate.
        //   mov     xxx, 00003FDFh
        if (*(ULONG*)(pCodePtr + i) == 0x00003FDF)
        {
            // Update the max PFN to use the arcade limit (128MB).
            Util_WriteDword(pCodePtr + i, 0x00007FCF);
            return;
        }
    }

    // If we made it here the instruction containing the max PFN wasn't found.
    DBG_BREAKPOINT();
}