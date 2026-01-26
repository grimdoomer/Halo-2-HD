
#pragma once
#include "Blam/Halo2.h"

// sizeof = 0x74
struct render_camera
{
    /* 0x00 */ // vector3 position?
    byte unk1[0x28];
    /* 0x28 */ float vertical_field_of_view;
    /* 0x2C */ float unk2;
    /* 0x30 */ rectangle_2d viewport_bounds;
    /* 0x38 */ rectangle_2d window_bounds;
    /* 0x40 */ float z_near;
    /* 0x44 */ float z_far;
    /* 0x48 */ byte unk3[0x2C];
};
static_assert(sizeof(render_camera) == 0x74, "render_camera incorrect struct size");

extern "C"
{
    // extern ... global_window_parameters;
    extern render_camera global_window_parameters__camera;

    extern float _anamorphic_scale_factor;
};