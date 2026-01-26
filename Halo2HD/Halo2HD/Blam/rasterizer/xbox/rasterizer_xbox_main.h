
#pragma once
#include "Blam/Halo2.h"
#include "Blam/DirectX.h"

struct rasterizer_device_parameters
{
    /* 0x00 */ bool _floating_point_depth_buffer;       // Indicates if the depth buffer format is D3DFMT_F24S8 (format is D3DFMT_D24S8 when false)
    /* 0x01 */ bool _present_immediately;               // Indicates if the next frame buffer will be presented immediately or wait for a vblank interval (vsync)
    /* 0x02 */ word _vblank_interval;                   // 0 - swap as soon as the frame is available, 1 - swap every screen refresh, 2 - swap every other screen refresh
    /* 0x04 */ unsigned int _flicker_filter_level;      // Flicker filter level [0, 5] used for D3DDevice_SetFlickerFilter
    /* 0x08 */ bool _soft_display_filter_enabled;       // Enables or disables soft display filter (see D3DDevice_SetSoftDisplayFilter)
};
static_assert(sizeof(rasterizer_device_parameters) == 0xC, "rasterizer_device_parameters incorrect struct size");

struct s_rasterizer_globals
{
    /* 0x00 */ bool initialize;
    /* 0x01 */ bool gpu_crashed;                                    // Only used in debug builds
    /* 0x02 */ rectangle_2d screen_bounds;
    /* 0x0A */ rectangle_2d frame_bounds;
    /* 0x18 */ LARGE_INTEGER _present_count;                        // Number of times rasterizer_present has been called
    /* 0x20 */ byte _pad1[8];
    /* 0x28 */ LARGE_INTEGER _vblank_count;                         // Number of times the vblank interrupt has been fired
    /* 0x30 */ LARGE_INTEGER _vblank_count_last;                    // Number of times the vblank interrupt has been fired at the time of a new frame swap
    /* 0x38 */ word _refresh_rate_hz;                               // Refresh rate in hertz, 60 for NTSC and PAL 60, 50 for PAL 50
    /* 0x3A */ bool _widescreen_enabled;
    /* 0x3B */ bool _letterbox_enabled;
    /* 0x3C */ bool _pal_video_mode;
    /* 0x3D */ bool _progressive_scan_enabled_old;                  // Same as _progressive_scan_enabled but only used in 1.0 xbe
    /* 0x3E */ bool _progressive_scan_enabled;
    /* 0x3F */ byte _pad2;
    /* 0x40 */ rasterizer_device_parameters _device_parameters;
    /* 0x4C */ float _near_clip_distance;                           // Depth clip near in world units
    /* 0x50 */ float _far_clip_distance;                            // Depth clip far in world units
    /* 0x54 */ float _near_clip_clamped;                            // Depth clip near clamped to the minimum supported by the depth buffer format (0f)
    /* 0x58 */ float _far_clip_clamped;                             // Depth clip far clamped to the maximum supported by the depth buffer format (16777215.0)
    /* 0x5C */ void* default_2d_bitmap_hardware_format;             // Default 2d bitmap (IDirect3DTexture8*)
    /* 0x60 */ void* default_3d_bitmap_hardware_format;             // Default 3d bitmap (IDirect3DVolumeTexture8*)
    /* 0x64 */ void* default_cm_bitmap_hardware_format;             // Default cubemap bitmap (IDirect3DCubeTexture8*)
};
static_assert(sizeof(s_rasterizer_globals) == 0x68, "s_rasterizer_globals incorrect struct size");

extern "C"
{
    extern s_rasterizer_globals rasterizer_globals;

    extern D3DDevice* global_d3d_device;

    extern unsigned long current_flicker_filter;
    extern unsigned long current_soft_display_filter;

    extern bool _rasterizer_recreate_primary_targets;

    extern D3DTexture* global_d3d_texture_render_primary[2];
    extern D3DSurface* global_d3d_surface_render_primary[2];
    extern D3DSurface* global_d3d_surface_render_primary_z;
    extern D3DTexture* global_d3d_texture_z_as_target[2];
    extern D3DSurface* global_d3d_surface_z_as_target;          // Note this is allocated with a size of 0x18 (4 bytes larger than D3DSurface), not sure why, may be a mistake...


    void _rasterizer_detect_video_mode();

    // void __usercall rasterizer_init_screen_bounds(__int16 x_off@<dx>, __int16 y_off@<cx>, float scale@<xmm0>);
    void __usercall __rasterizer_init_screen_bounds(word x_off, word y_off, float scale);

    bool rasterizer_preinitialize();

    // bool __usercall rasterizer_device_initialize@<al>(const struct rasterizer_device_parameters* parameters@<eax>)
    bool __usercall _rasterizer_device_initialize(const rasterizer_device_parameters* parameters);

    bool rasterizer_primary_targets_initialize();

    bool _rasterizer_should_render_screen_effect();
};

void __cdecl _rasterizer_init_screen_bounds(word x_off, word y_off, float scale)
{
    _asm
    {
        movss   xmm0, scale
        movsx   ecx, y_off
        movsx   edx, x_off
        call    __rasterizer_init_screen_bounds
    }
}

bool __cdecl rasterizer_device_initialize(const rasterizer_device_parameters* parameters)
{
    _asm
    {
        mov     eax, parameters
        call    _rasterizer_device_initialize
    }
}