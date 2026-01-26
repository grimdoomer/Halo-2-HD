
#pragma once
#include "Blam/Halo2.h"

// sizeof = 0x1D0
struct font_header
{

};

enum
{
    k_maximum_font_index_count = 10
};

enum font_id
{

};

enum internal_font_id
{
    
};

extern "C"
{
    extern internal_font_id _font_id_table[k_maximum_font_index_count + 1];

    // font_header *__usercall font_get_header_internal@<eax>(internal_font_id internal_id@<ecx>)
    font_header* __usercall _font_get_header_internal(internal_font_id internal_id);
};

font_header* font_get_header_internal(internal_font_id internal_id)
{
    // font_header *__usercall font_get_header_internal@<eax>(internal_font_id internal_id@<ecx>)

    _asm
    {
        mov     ecx, internal_id
        call    _font_get_header_internal
    }
}

font_header* font_get_header(font_id font_id)
{
    return font_get_header_internal(_font_id_table[font_id]);
}