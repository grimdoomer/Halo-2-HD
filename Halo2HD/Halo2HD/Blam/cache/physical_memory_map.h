
#pragma once

#define k_physical_memory_page_mask				PAGE_SIZE - 1
#define k_physical_memory_allocation_mask		0x7FFFFFFF

enum
{
	_memory_stage_initial = 0,
	// 1 = rasterizer initialization
	// 2 = sub-system initialization
	// 3 = per-map data (tags, unicode strings, bitmaps)
	_memory_stage_max = 5
};

// sizeof = 0x1C
struct s_physical_memory_journal_entry
{
	/* 0x00 */ unsigned int stage;								// Stage the block was allocated during
};

#define _k_physical_memory_base_address			0x00061000		// Base address for the physical memory region allocation
#define _k_physical_memory_region_size			0x030E4000		// ~48.9 MB

#define _k_physical_memory_address_mask			0x80000000		// Mask that must be applied to memory addresses returned from MmAllocateContiguousMemoryEx

// sizeof = 0x34
struct s_physical_memory_globals
{
	/* 0x00 */ unsigned int current_stage;								// See enum above
	/* 0x04 */ void* _start_address;									// Starting address for the runtime data memory region (0x00061000)
	/* 0x08 */ void* _end_address;										// Ending address for the runtime data memory region (0x03145000)
	/* 0x0C */ void* low_stage_address[5];								// Per-stage memory region starting address
	/* 0x20 */ void* hi_stage_address[5];								// Per-stage memory region end address

#ifdef _DEBUG
	/* 0x00 */ s_physical_memory_journal_entry _journal_entries[64];	// Only used in debug builds
#endif
};

extern "C"
{
	extern s_physical_memory_globals physical_memory_globals;

	void physical_memory_initialize();
};
