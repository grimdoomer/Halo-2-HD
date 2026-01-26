
#pragma once
#include "Blam/Halo2.h"

extern "C"
{
    // void __usercall draw_quad(struct rectangle2d *rect@<ecx>, unsigned int color@<esi>)
    void __usercall _draw_quad(rectangle_2d* rect, unsigned int color);
};

void __usercall draw_quad(rectangle_2d* rect, unsigned int color)
{
    _asm
    {
        mov     esi, color
        mov     ecx, rect
        call    _draw_quad
    }
}