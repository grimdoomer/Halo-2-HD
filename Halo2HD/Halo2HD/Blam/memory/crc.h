
#pragma once
#include "Blam/Halo2.h"

extern "C"
{
    // void __usercall crc_checksum_buffer(int* checksum, unsigned char* buffer@<eax>, int length@<edi>);
    void __usercall _crc_checksum_buffer(int* checksum, unsigned char* buffer, int length);
};

void crc_checksum_buffer(int* checksum, unsigned char* buffer, int _length)
{
    // void __usercall crc_checksum_buffer(int* checksum, unsigned char* buffer@<eax>, int length@<edi>);

    _asm
    {
        // Save edx as crc_checksum_buffer will trash it.
        push    edx

        mov     edi, _length
        mov     eax, buffer
        push    checksum
        call    _crc_checksum_buffer

        pop     edx
    }
}