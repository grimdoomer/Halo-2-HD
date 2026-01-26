
#pragma once
#include "Blam/Halo2.h"

struct s_data_array
{
    /* 0x00 */ char name[32];
    /* 0x20 */ int maximum_count;
    /* 0x24 */ int size;                                // Size of a single element
    /* 0x28 */ byte alignment_bits;
    /* 0x29 */ bool _valid;
    /* 0x2A */ byte unk2;
    /* 0x2C */ group_tag signature;
    /* 0x30 */ int unk3;
    /* 0x34 */ int unk4;
    /* 0x38 */ int unk5;
    /* 0x3C */ int _entry_count;                        // Number of entries currently in use
    /* 0x40 */ word _next_datum;                        // Next datum value to use
    /* 0x44 */ void* data_ofs;                          // Pointer to data start
    /* 0x48 */ void* in_use_bit_vector_address;         // Pointer to the in use bit vector
};
static_assert(sizeof(s_data_array) == 0x4C, "s_data_array incorrect struct size");

extern "C"
{
    // void __usercall data_delete_all(struct s_data_array *array@<esi>);
    void _data_delete_all(s_data_array* array);
};

void data_delete_all(s_data_array* array)
{
    _asm
    {
        mov     esi, array
        call    _data_delete_all
    }
}

void data_make_valid(s_data_array* array)
{
    data_delete_all(array);
    array->_valid = true;
}

void data_make_invalid(s_data_array* array)
{
    array->_valid = false;
}