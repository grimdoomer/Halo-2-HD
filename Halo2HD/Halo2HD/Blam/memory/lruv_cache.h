
#pragma once
#include "Blam/Halo2.h"
#include "Blam/memory/data.h"

typedef void (*lruv_cache_delete_block_proc)(datum_index datum);
typedef bool (*lruv_cache_locked_block_proc)(datum_index datum);
typedef char (*lruv_cache_usage_block_proc)(datum_index datum);

// sizeof = 0x70
struct lruv_cache
{
	/* 0x00 */ char name[32];						// Name of the cache table
	/* 0x20 */ lruv_cache_delete_block_proc delete_block_proc;
	/* 0x24 */ lruv_cache_locked_block_proc locked_block_proc;
	/* 0x28 */ lruv_cache_usage_block_proc usage_block_proc;

	/* 0x30 */ //DWORD cache_size;					// Size of cache >> 12
	/* 0x34 */ DWORD frame_index;
	/* 0x38 */ DWORD element_count;
	/* 0x3C */ DWORD first_block_index;
	/* 0x40 */ DWORD last_block_index;

	// 0x44-0x60 get set to 0x7FFFFFFF? 0x12C5DE

	/* 0x64 */ s_data_array* array;
	/* 0x68 */ group_tag signature;					// 'weee'
	/* 0x6C */ // allocation vtable ptr

	/* 0x70 */ // array follows here
};

struct lruv_cache_block
{
	/* 0x00 */ unsigned int unk1;
	/* 0x04 */ unsigned int page_count;
	/* 0x08 */ unsigned int first_page_index;
	/* 0x0C */ unsigned int next_block_index;
	/* 0x10 */ unsigned int previous_block_index;
	/* 0x14 */ unsigned int last_used_frame_index;		// Set to 0x38 on lruv cache
};
static_assert(sizeof(lruv_cache_block) == 0x18, "lruv_cache_block incorrect struct size");

extern "C"
{
	// void __usercall lruv_resize(struct lruv_cache *cache@<eax>, int size);
	void __usercall _lruv_resize(lruv_cache* cache, int size_in_pages);
};

void __cdecl lruv_resize(lruv_cache* cache, int size_in_pages)
{
	_asm
	{
		mov		eax, cache
		push	size_in_pages
		call	_lruv_resize
	}
}