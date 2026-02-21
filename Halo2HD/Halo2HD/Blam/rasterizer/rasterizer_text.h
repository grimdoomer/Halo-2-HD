
#pragma once
#include "Blam/Halo2.h"

extern "C"
{
    // void __usercall _rasterizer_draw_string(const wchar_t* string@<ecx>, rectangle_2d* bounds, rectangle_2d* unk1, int unk2, int unk3, float unk4, int unk5, int unk6);
    void __usercall _rasterizer_draw_string(const wchar_t* string, rectangle_2d* bounds, rectangle_2d* unk1, int unk2, int unk3, float unk4, int unk5, int unk6);
};

void rasterizer_draw_string(const wchar_t* string, rectangle_2d* bounds, rectangle_2d* unk1, int unk2, int unk3, float unk4, int unk5, int unk6)
{
    // void __usercall _rasterizer_draw_string(const char* string@<ecx>, rectangle_2d* bounds, rectangle_2d* unk1, int unk2, int unk3, float unk4, int unk5, int unk6);

    _asm
    {
        push    unk6
        push    unk5
        push    unk4
        push    unk3
        push    unk2
        push    unk1
        push    bounds
        mov     ecx, string
        call    _rasterizer_draw_string
    }
}