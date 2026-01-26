
#pragma once
#include "Blam/Halo2.h"
#include "Blam/DirectX.h"

enum
{
	// _rasterizer_target_backbuffer = 0 ?
	_rasterizer_target_texaccum = 1,
	k_rasterizer_target_count = 39
};

enum
{
	_rasterizer_target_type_static = 1,

	_rasterizer_target_type_texture_cache_resident = 3,
	k_rasterizer_target_type_count = 4
};

enum
{
	k_rasterizer_target_max_mipmaps = 8
};

// sizeof = 0x98
struct _s_rasterizer_render_target
{
	/* 0x00 */ unsigned int type;									// See _rasterizer_target_type_* above
	/* 0x04 */ D3DPixelContainer _primary_surface;					// Main render target
	/* 0x18 */ D3DPixelContainer d3d_surface_storage[4];			// Pixel containers for sub surfaces?
	/* 0x68 */ byte unk1[0x10];
	/* 0x78 */ unsigned int mipmap_count;
	/* 0x7C */ unsigned int allocated_pixel_data_size_in_bytes;
	/* 0x80 */ unsigned int width;
	/* 0x84 */ unsigned int height;
	/* 0x88 */ void* data;											// Pointer to memory for the texture buffer (if self allocated?)
	/* 0x8C */ unsigned int data_offset_in_bytes;
	/* 0x90 */ unsigned int unk2;
	/* 0x94 */ bool linear;
	/* 0x95 */ bool pending_deallocation;							// Indicates the render target buffer was allocated with memory stolen from the texture cache (type must be _rasterizer_target_type_texture_cache_resident)
};
static_assert(sizeof(_s_rasterizer_render_target) == 0x98, "_s_rasterizer_render_target incorrect struct size");


extern "C"
{
	extern _s_rasterizer_render_target _rasterizer_render_targets[k_rasterizer_target_count];

	// bool __usercall __rasterizer_create_render_target@<al>(int rasterizer_target@<eax>, int type, int width, int height, bool z_surface, bool linear, int, int buffer)
	bool __usercall __rasterizer_create_render_target(int rasterizer_target, int type, int width, int height, bool z_surface, bool linear, int unk, void* buffer);

	// bool __usercall __rasterizer_allocate_and_create_render_target@<al>(int rasterizer_target, int width, int height@<ebx>, bool, bool z_surface)
	bool __usercall __rasterizer_allocate_and_create_render_target(int rasterizer_target, int width, int height, bool unk, bool z_surface);

	// bool __usercall rasterizer_get_render_target_resolution@<al>(int rasterizer_target@<eax>, int *resolution_x@<edx>, int *resolution_y@<ecx>)
	bool __usercall __rasterizer_get_render_target_resolution(int rasterizer_target, int* resolution_x, int* resolution_y);
};

bool _rasterizer_create_render_target(int rasterizer_target, int _type, int width, int height, bool z_surface, bool linear, int unk, void* buffer)
{
	_asm
	{
		push	buffer
		push	unk
		push	linear
		push	z_surface
		push	height
		push	width
		push	_type
		mov		eax, rasterizer_target
		call	__rasterizer_create_render_target
	}
}

bool _rasterizer_allocate_and_create_render_target(int rasterizer_target, int width, int height, bool unk, bool z_surface)
{
	_asm
	{
		push	z_surface
		push	unk
		mov		ebx, height
		push	width
		push	rasterizer_target
		call	__rasterizer_allocate_and_create_render_target
	}
}

bool _rasterizer_get_render_target_resolution(int rasterizer_target, int* resolution_x, int* resolution_y)
{
	_asm
	{
		mov		ecx, resolution_y
		mov		edx, resolution_x
		mov		eax, rasterizer_target
		call	__rasterizer_get_render_target_resolution
	}
}