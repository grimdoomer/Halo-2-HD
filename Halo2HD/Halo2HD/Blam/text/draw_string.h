
#pragma once
#include "Blam/Halo2.h"
#include "Blam/text/font_group.h"

enum
{
    k_text_style_count = 4,
    k_text_justifications_count = 3,
    k_maximum_number_of_tab_stops = 16,

    _text_style_plain = 0xffffffff,
};

struct s_font_drawing_globals
{
    /* 0x00 */ int font;
    /* 0x04 */ int flags;
    /* 0x08 */ int style;
    /* 0x0C */ int justification;
    /* 0x10 */ real_argb_color color;
    /* 0x20 */ bool _use_shadow_color;
    /* 0x24 */ real_argb_color shadow_color;
    /* 0x34 */ int unk1;                                            // Some type of function pointer?
    /* 0x38 */ int unk2;                                            // Possibly input for the function pointer above?
    /* 0x3C */ word tab_stop_count;
    /* 0x3E */ word _tab_stops[k_maximum_number_of_tab_stops];
    /* 0x5E */ word initial_indent;
    /* 0x60 */ word paragraph_indent;
};
static_assert(sizeof(s_font_drawing_globals) == 0x64, "s_font_drawing_globals incorrect struct size");

extern "C"
{
    extern s_font_drawing_globals font_drawing_globals;

    void draw_string_reset_state();

    // void __usercall draw_string_set_color_0(struct real_argb_color *color@<eax>)
    void __usercall _draw_string_set_color(real_argb_color* color);

    // void __usercall draw_string_set_shadow_color(struct real_argb_color *shadow_color@<eax>)
    void __usercall _draw_string_set_shadow_color(real_argb_color* shadow_color);
};

void draw_string_set_color(real_argb_color* color)
{
    // void __usercall draw_string_set_color_0(struct real_argb_color *color@<eax>)

    _asm
    {
        mov     eax, color
        call    _draw_string_set_color
    }
}

void draw_string_set_shadow_color(real_argb_color* shadow_color)
{
    // void __usercall draw_string_set_shadow_color(struct real_argb_color *shadow_color@<eax>)

    _asm
    {
        mov     eax, shadow_color
        call    _draw_string_set_shadow_color
    }
}

void draw_string_set_font(font_id font_id)
{
    font_get_header(font_id);
    font_drawing_globals.font = font_id;
}

void draw_string_set_format(int style, int justification, int flags)
{
    font_drawing_globals.style = style;
    font_drawing_globals.justification = justification;
    font_drawing_globals.flags = flags;
}