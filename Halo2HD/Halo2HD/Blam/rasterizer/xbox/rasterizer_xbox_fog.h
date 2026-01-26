
#pragma once
#include "Blam/Halo2.h"

extern "C"
{
    bool rasterizer_fog_composite_get_register(int register_index, real_rectangle_2d* bounds, real_vector_2d* location, real_vector_4d* output, void* unk);
};