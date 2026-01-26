
#pragma once
#include "Blam/Halo2.h"

extern "C"
{
    // Do NOT call, use crc32_calculate instead
    void crc_checksum_buffer(int* checksum, unsigned char* buffer, int length);

    // Stub for fastcall version of crc_checksum_buffer
    void __cdecl crc32_calculate(int* checksum, unsigned char* buffer, int length);
};

// ------------------------------------------------------------------------------
// Debugging utilities:

#define DBG_BREAKPOINT()    _asm { int 3 }

// ------------------------------------------------------------------------------
// Helper functions:

inline float Util_DegreesToRadians(float value)
{
    return value * 0.01745f;
}

inline int clamp(int val, int min, int max)
{
    return max(min(val, max), min);
}

// ------------------------------------------------------------------------------
// Utilities.cpp:
void lstrcpyA(char* dst, char* src);

void Util_InstallHook(void* address, void* detour);
void Util_WriteDword(void* address, unsigned int value);

ULONG Util_GetMemoryCapacity();

void Util_SetFanSpeed(int percent);

void Util_OverclockGPU(int step);

bool Util_AtaIdentify(BYTE* pIdentifyData);
bool Util_WaitForHddReady(int* pStatus);
bool Util_HddSetTransferSpeed(int mode);