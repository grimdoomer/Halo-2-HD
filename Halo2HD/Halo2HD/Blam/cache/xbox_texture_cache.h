
#pragma once
#include "Halo2HD.h"
#include "Blam/memory/lruv_cache.h"

struct _s_texture_cache_globals
{
    /* 0x00 */ s_data_array* textures;
    /* 0x04 */ s_data_array* predicted_textures;
    /* 0x08 */ void* stolen_blocks;                             // Not sure what the structure of this is just yet, seems to be a linked list
    /* 0x0C */ void* standard_cache_base_address;
    /* 0x10 */ lruv_cache* standard_cache;
    /* 0x14 */ bool _low_detail_cache_initialized;              // Indicates if the low detail texture cache has been initialized
    /* 0x18 */ void* low_detail_cache_base_address;
    /* 0x1C */ unsigned int unk1;
    /* 0x20 */ unsigned int unk2;
    /* 0x24 */ bool stolen_memory;
    /* 0x25 */ bool unk4;
    /* 0x26 */ bool unk5;
    /* 0x28 */ float unk6;                                      // Something to do with lod selection
    /* 0x2C */ unsigned int unk7;                               // Some kind of time stamp
    /* 0x30 */ unsigned int unk8;                               // Number of textures cache per frame?
    /* 0x24 */ unsigned int unk9;
};
static_assert(sizeof(_s_texture_cache_globals) == 0x38, "_s_texture_cache_globals incorrect struct size");

// sizeof = 0x28
struct texture_datum
{
    /* 0x00 */ word datum;
    /* 0x02 */ // BYTE stolen;
    /* 0x03 */ // BYTE initialized? loaded? locked?
    /* 0x04 */ DWORD lod;			                    // Lod level for the bitmap (0-2)
    /* 0x08 */ // DWORD some sort of flags?
    /* 0x0C */ //DWORD return value from loading bitmap raw (0x0012D6A5), some sort of load id/index?
    /* 0x10 */ void* bitmap_data;                       // bitmap_data_block*
    /* 0x14 */ D3DPixelContainer d3d_texture;	        // D3D texture data (sizeof = 0x14)
};

struct predicted_texture_datum
{
    /* 0x00 */ word datum;
    /* 0x04 */ void* bitmap_data;                       // bitmap_data_block*
};
static_assert(sizeof(predicted_texture_datum) == 8, "predicted_texture_datum incorrect struct size");


extern "C"
{
    extern _s_texture_cache_globals xbox_texture_cache_globals;

    void texture_cache_open();
    void texture_cache_close();
    void texture_cache_flush();
};