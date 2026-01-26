
#pragma once
#include "Halo2HD.h"
#include "Blam/memory/lruv_cache.h"

// I think sizeof = 0x28 on release builds, debug builds have additional data for debug UI strings and such...
struct _s_geometry_cache_globals
{
    /* 0x00 */ s_data_array* geometry_blocks;
    /* 0x04 */ s_data_array* predicted_geometry;
    /* 0x08 */ void* cache_base_address;
    /* 0x0C */ unsigned int cache_size;
    /* 0x10 */ lruv_cache* cache;
    /* 0x14 */ unsigned int unk1;
    /* 0x18 */ unsigned int unk2;
    /* 0x1C */ unsigned int _missed_level_geometry_reads;       // Number of cache misses for level geometry blocks
    /* 0x20 */ unsigned int unk3;                               // Some sort of time stamp
    /* 0x24 */ unsigned char unk4;
};

extern "C"
{
    extern _s_geometry_cache_globals xbox_geometry_cache_globals;

    void geometry_cache_open(int unknown);
    void geometry_cache_close();
};