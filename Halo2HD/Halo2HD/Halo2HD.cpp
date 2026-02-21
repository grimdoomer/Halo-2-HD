// Halo2HD.cpp : Defines the functions for the static library.
//

#include "Halo2HD.h"
#include "Xbox/Xbox.h"
#include "Blam/Halo2.h"
#include "Config/Config.h"
#include "Utilities/Utilities.h"
#include <intrin.h>

#include "Blam/DirectX.h"
#include "Blam/XGraphics.h"

#include "Blam/bink/bink_playback.h"
#include "Blam/cache/physical_memory_map.h"
#include "Blam/cache/xbox_geometry_cache.h"
#include "Blam/cache/xbox_texture_cache.h"
#include "Blam/camera/first_person_camera.h"
#include "Blam/cutscene/cinematics.h"
#include "Blam/interface/interface.h"
#include "Blam/render/render_cameras.h"
#include "Blam/render/renderer.h"
#include "Blam/rasterizer/xbox/rasterizer_xbox_fog.h"
#include "Blam/rasterizer/xbox/rasterizer_xbox_main.h"
#include "Blam/rasterizer/xbox/rasterizer_xbox_targets.h"

// Region indices for Hack_PhysicalMemoryRegionInfoTable:
#define PHYS_MEM_REGION_GEOMETRY_CACHE                                  0
#define PHYS_MEM_REGION_TEXTURE_CACHE                                   1
#define PHYS_MEM_REGION_RASTERIZER_TEXACCUM_TARGET                      2


// Runtime cache sizes:
#define RUNTIME_DATA_REGION_SIZE_ADJUST_128MB   (1024 * 1024 * 25)      // 25MB size reduction for 128MB consoles
#define RUNTIME_DATA_REGION_SIZE_ADJUST_64MB    (1024 * 1024 * 1)       // 1MB size reduction for 64MB consoles

#define EXPANDED_TEXTURE_CACHE_SIZE             (1024 * 1024 * 25)      // 25MB
#define EXPANDED_TEXTURE_CACHE_SIZE_720         (1024 * 1024 * 30)      // 30MB

#define EXPANDED_GEOMETRY_CACHE_SIZE            (1024 * 1024 * 15)      // 15MB(SP = 6.5MB, MP = 7MB)
#define EXPANDED_GEOMETRY_CACHE_SIZE_720        (1024 * 1024 * 20)      // 20MB


// Physical memory descriptors for remapped regions of the game data allocation:
struct PhysicalMemoryRegionDescriptor
{
    const char* Name;       // Name of the memory region
    unsigned int Size;      // Size of the region
    void* BaseAddress;      // Base address of the region (or null if not allocated)
};

PhysicalMemoryRegionDescriptor Hack_PhysicalMemoryRegionInfoTable[] =
{
    { "geometry cache",                 0,  nullptr },
    { "texture cache",                  0,  nullptr },
    { "rasterizer texaccum target",     0,  nullptr }
};

enum
{
    PhysMemRegion_GeometryCache = 0,
    PhysMemRegion_TextureCache,
    PhysMemRegion_RasterizerTexaccumTarget
};


bool Hack_HasRamUpgrade = false;
bool Hack_TripleBufferingEnabled = false;

bool Hack_RasterizerTargetsInitialized = false;

unsigned int Hack_RuntimeDataRegionSize = _k_physical_memory_region_size;        // ~48.9 MB


// Forward declarations:
bool Hook_rasterizer_preinitialize();
bool Hook_rasterizer_device_initialize(const rasterizer_device_parameters* parameters);
ULONG Hook_D3DDevice_Swap(ULONG Flags);
void Hook_physical_memory_initialize();
void Hook_rasterizer_detect_video_mode();
void Hook_rasterizer_init_screen_bounds(/* word x_off, word y_off, float scale */);
bool Hook_rasterizer_primary_targets_initialize();
bool Hook_rasterizer_allocate_and_create_render_target(int rasterizer_target, int width, /* int height, */ bool unk, bool z_surface);
bool Hook_rasterizer_get_render_target_resolution(/* int rasterizer_target, int* resolution_x, int* resolution_y */);
void Hook_create_render_target_18_helper();
bool Hook_rasterizer_should_render_screen_effect();
bool Hook_rasterizer_fog_composite_get_register(int register_index, real_rectangle_2d* bounds, real_vector_2d* location, real_vector_4d* output, void* unk);

void Hook_interface_splitscreen_render();
void Hook__renderer_setup_player_windows();

void Hook_geometry_cache_open(int unknown);
void Hook_geometry_cache_close();

void Hook_texture_cache_open();
void Hook_texture_cache_close();

void* Hack_PhysicalMemoryAlloc(int regionIndex, int size, int protect);
void Hack_PhysicalMemoryFree(int regionIndex);

void Hook_bink_playback_start(const char* file_path, unsigned int flags);

bool Hook__should_draw_player_hud();

void Hack_ColdRebootConsole();

extern void Dbg_DrawPerfCounters();


extern "C" void __cdecl ExpansionMain()
{
    // Resolve kernel imports.
    ResolveKernelImports();

    // Print startup banner.
    DbgPrint("Halo 2 HD initializing...\n");
    //DBG_BREAKPOINT();

    // Parse the config file.
    Cfg_ParseConfigFile();

    // Check if we should overclock the GPU.
    if (Cfg_OverclockGPU.GetValue<bool>() == true)
    {
        // Check that the fan speed override has been set and force it to max if not.
        if (Cfg_OverrideFanSpeed.GetValue<bool>() == false)
        {
            // Force fan speed to max because the user is an idiot and didn't read the big fat
            // warnings about cooking the GPU on stock fan speeds. If they choose to enabled the
            // override and not set the fan speed high enough then RIP to their GPU.
            Cfg_FanSpeedPercent.SetValue<int>(100);
        }

        // Set the fan speed first.
        Util_SetFanSpeed(Cfg_FanSpeedPercent.GetValue<int>());

        // Update GPU clock configuration.
        Util_OverclockGPU(Cfg_GPUOverclockStep.GetValue<int>());
    }

    // Check if we should set the HDD speed.
    if (Cfg_SetHddSpeed.GetValue<bool>() == true)
    {
        // Set HDD UDMA speed.
        if (Util_HddSetTransferSpeed(Cfg_HddSpeed.GetValue<int>()) == false)
        {
            // Set HDD speed failed, console could be in unstable state, do a cold reboot.
            HalReturnToFirmware(3);
        }
    }

    // Check if the console has 128MB of RAM.
    if (Util_GetMemoryCapacity() == 128)
    {
        // Patch the max usable PFN for contiguous allocations and flag the console has 128MB of RAM.
        PatchkernelPhysicalMemoryLimit();
        Hack_HasRamUpgrade = true;

        // Force enable triple buffering.
        Hack_TripleBufferingEnabled = true;

        // Adjust the size of the runtime data region since we'll be moving the texture and geometry cache out.
        Hack_RuntimeDataRegionSize -= RUNTIME_DATA_REGION_SIZE_ADJUST_128MB;

        // Hook physical memory and cache initialization functions.
        Util_InstallHook((void*)geometry_cache_open, (void*)Hook_geometry_cache_open);
        Util_InstallHook((void*)geometry_cache_close, (void*)Hook_geometry_cache_close);
        Util_InstallHook((void*)texture_cache_open, (void*)Hook_texture_cache_open);
        Util_InstallHook((void*)texture_cache_close, (void*)Hook_texture_cache_close);
    }
    else
    {
        // Install hooks specific to consoles with no RAM upgrade.

        // Adjust the size of the runtime data region to account for increased front/back buffers.
        Hack_RuntimeDataRegionSize -= RUNTIME_DATA_REGION_SIZE_ADJUST_64MB;
    }

    // Install hooks.
    Util_InstallHook((void*)physical_memory_initialize, (void*)Hook_physical_memory_initialize);
    Util_InstallHook((void*)_D3DDevice_Swap, (void*)Hook_D3DDevice_Swap);
    Util_InstallHook((void*)_rasterizer_detect_video_mode, (void*)Hook_rasterizer_detect_video_mode);
    Util_InstallHook((void*)__rasterizer_init_screen_bounds, (void*)Hook_rasterizer_init_screen_bounds);
    Util_InstallHook((void*)rasterizer_preinitialize, (void*)Hook_rasterizer_preinitialize);
    Util_InstallHook((void*)_rasterizer_device_initialize, (void*)Hook_rasterizer_device_initialize);
    Util_InstallHook((void*)rasterizer_primary_targets_initialize, (void*)Hook_rasterizer_primary_targets_initialize);
    Util_InstallHook((void*)__rasterizer_allocate_and_create_render_target, (void*)Hook_rasterizer_allocate_and_create_render_target);
    Util_InstallHook((void*)__rasterizer_get_render_target_resolution, (void*)Hook_rasterizer_get_render_target_resolution);
    Util_InstallHook((void*)_rasterizer_should_render_screen_effect, (void*)Hook_rasterizer_should_render_screen_effect);

    Util_InstallHook((void*)interface_splitscreen_render, (void*)Hook_interface_splitscreen_render);

    Util_InstallHook((void*)bink_playback_start, (void*)Hook_bink_playback_start);

#ifdef H2_1_0
    Util_InstallHook((void*)VERSION_SPECIFIC_ADDR(0x0001DC40), (void*)Hook_create_render_target_18_helper);
    Util_WriteDword((void*)VERSION_SPECIFIC_ADDR(0x00025E09), (unsigned int)Hook_rasterizer_fog_composite_get_register);
    Util_InstallHook((void*)VERSION_SPECIFIC_ADDR(0x00223564), (void*)Hook__renderer_setup_player_windows);
#elif H2_1_5
    Util_InstallHook((void*)VERSION_SPECIFIC_ADDR(0x0001DC20), (void*)Hook_create_render_target_18_helper);
    Util_WriteDword((void*)VERSION_SPECIFIC_ADDR(0x00025DE9), (unsigned int)Hook_rasterizer_fog_composite_get_register);
    Util_InstallHook((void*)VERSION_SPECIFIC_ADDR(0x00229F74), (void*)Hook__renderer_setup_player_windows);
#endif
    
    // Hook HalReturnToFirmware so that any attempt to quit the game results in a cold reboot of the console.
    // Due to how we hot patch the kernel(in 128MB mode) the console is basically "hosed" after running the game. Any attempt
    // to run another executable, return to dash, IRG, etc, will result in graphical artifacting and the console freezing.
    // If the console doesn't have extra RAM but the user overclocks the GPU we want to reset that as well.
    Util_InstallHook((void*)HalReturnToFirmware, (void*)Hack_ColdRebootConsole);

    // Calculate fov scale.
    _first_person_field_of_view_scale = Cfg_FieldOfView.GetValue<float>() / 70.f;

    // Check if we should disable the HUD.
    if (Cfg_DisableHud.GetValue<bool>() == true)
    {
#ifdef H2_1_0
        Util_InstallHook((void*)VERSION_SPECIFIC_ADDR(0x0013939B), (void*)Hook__should_draw_player_hud);
#elif H2_1_5
        Util_InstallHook((void*)VERSION_SPECIFIC_ADDR(0x0013BE7B), (void*)Hook__should_draw_player_hud);
#endif
    }

    // Check if we should disable atmospheric fog for more perf.
    if (Cfg_DisableAtmosphericFog.GetValue<bool>() == true)
    {
        // TODO
    }

    // Check if we should disable anamorphic scaling.
    if (Cfg_DisableAnamorphicScaling.GetValue<bool>() == true)
    {
        // Set anamorphic scale factor to 1.0 (_anamorphic_scale_factor is in rdata section...).
        float newScale = 1.f;
        Util_WriteDword(&_anamorphic_scale_factor, *(unsigned int*)&newScale);
    }

    // Call the game's main routine.
    game_main(0, 0, 0);

    // Should never get here but just in case, return to dash.
    XapiBootToDash(1, 1, 0);
}

bool Hook_rasterizer_preinitialize()
{
    // Setup device present parameters.
    D3DPRESENT_PARAMETERS PresentParams = { 0 };
    PresentParams.BackBufferWidth = 640;
    PresentParams.BackBufferHeight = 480;
    PresentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
    PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    PresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    PresentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    PresentParams.FullScreen_RefreshRateInHz = rasterizer_globals._refresh_rate_hz;
    PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Set various flags based on video settings.
    if (rasterizer_globals._widescreen_enabled == true)
        PresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN;
    if (rasterizer_globals._progressive_scan_enabled == true)
        PresentParams.Flags |= D3DPRESENTFLAG_PROGRESSIVE;

    // Check if 1080i support is enabled.
    if (Cfg_Enable1080iSupport.GetValue<bool>() == true)
    {
        // Check the console video flags and check if 1080i is enabled.
        if ((XGetVideoFlags() & XC_VIDEO_FLAGS_HDTV_1080i) != 0)
        {
            // Enabled interlaced mode.
            PresentParams.Flags &= ~D3DPRESENTFLAG_PROGRESSIVE;
            PresentParams.Flags |= D3DPRESENTFLAG_INTERLACED;
        }
    }

    // Create the d3d device.
    DbgPrint("rasterizer_preinitialize Width=%d Height=%d Flags=0x%08x\n", PresentParams.BackBufferWidth, PresentParams.BackBufferHeight, PresentParams.Flags);
    Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, nullptr, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresentParams, &global_d3d_device);
    if (global_d3d_device == nullptr)
    {
        // Failed to create d3d device.
        global_d3d_device = nullptr;
        return false;
    }

    // TODO: the game calls D3DDevice_GetDeviceCaps but the device caps data is never used
    return true;
}

bool _Hook_rasterizer_device_initialize(const rasterizer_device_parameters* parameters)
{
    // Copy device parameters to the global rasterizer data.
    memcpy(&rasterizer_globals._device_parameters, parameters, sizeof(rasterizer_device_parameters));

    // Initialize device present parameters.
    D3DPRESENT_PARAMETERS PresentParams = { 0 };
    PresentParams.BackBufferWidth = rasterizer_globals.screen_bounds.x1 - rasterizer_globals.screen_bounds.x0;
    PresentParams.BackBufferHeight = rasterizer_globals.screen_bounds.y1 - rasterizer_globals.screen_bounds.y0;
    PresentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
    PresentParams.AutoDepthStencilFormat = (D3DFORMAT)(D3DFMT_D24S8 + (rasterizer_globals._device_parameters._floating_point_depth_buffer == true ? 1 : 0));
    PresentParams.EnableAutoDepthStencil = TRUE;
    PresentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    PresentParams.FullScreen_RefreshRateInHz = rasterizer_globals._refresh_rate_hz;

    // Check the vblank interval and handle accordingly.
    if (rasterizer_globals._device_parameters._vblank_interval == 0)
    {
        // Vsync disabled:
        PresentParams.SwapEffect = D3DSWAPEFFECT_FLIP;
        PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    else if (rasterizer_globals._device_parameters._vblank_interval == 1)
    {
        // Vsync or immediately:
        PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        PresentParams.FullScreen_PresentationInterval = rasterizer_globals._device_parameters._present_immediately == true ? D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE : D3DPRESENT_INTERVAL_ONE;
    }
    else if (rasterizer_globals._device_parameters._vblank_interval == 2)
    {
        // Vsync or immediately (half frame rate):
        PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        PresentParams.FullScreen_PresentationInterval = rasterizer_globals._device_parameters._present_immediately == true ? D3DPRESENT_INTERVAL_TWO_OR_IMMEDIATE : D3DPRESENT_INTERVAL_TWO;
    }
    else
    {
        rasterizer_globals._device_parameters._vblank_interval = 1;
        PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }

    // Set various flags based on video settings.
    if (rasterizer_globals._widescreen_enabled == true)
        PresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN;
    if (rasterizer_globals._progressive_scan_enabled == true)
        PresentParams.Flags |= D3DPRESENTFLAG_PROGRESSIVE;

    // Check the resolution to see if 1080i is enabled.
    if (rasterizer_globals.screen_bounds.x1 == 1920)
    {
        // Enabled interlaced mode.
        PresentParams.Flags &= ~D3DPRESENTFLAG_PROGRESSIVE;
        PresentParams.Flags |= D3DPRESENTFLAG_INTERLACED;
    }

    // If triple buffering is enabled change the present interval and buffer count.
    if (Hack_TripleBufferingEnabled == true)
    {
        PresentParams.BackBufferCount = 2;
        PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        PresentParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }

    // If the device has already been created reset it using the new present parameters, else create it anew.
    DbgPrint("rasterizer_device_initialize Width=%d Height=%d Flags=0x%08x\n", PresentParams.BackBufferWidth, PresentParams.BackBufferHeight, PresentParams.Flags);
    if (global_d3d_device == nullptr)
        Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, nullptr, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresentParams, &global_d3d_device);
    else
        D3DDevice_Reset(&PresentParams);

    if (global_d3d_device == nullptr)
    {
        // Failed to create d3d device.
        global_d3d_device = nullptr;
        return false;
    }

    // TODO: the game calls D3DDevice_GetDeviceCaps but the device caps data is never used

    // Initialize flicker filter settings.
    clamp(rasterizer_globals._device_parameters._flicker_filter_level, 0, 5);
    current_flicker_filter = 0xFFFFFFFF;
    current_soft_display_filter = 0xFFFFFFFF;

    // Flag that the primary render targets need to be initialized.
    _rasterizer_recreate_primary_targets = true;

    return true;
}

bool __declspec(naked) Hook_rasterizer_device_initialize(const rasterizer_device_parameters* parameters)
{
    // bool __usercall rasterizer_device_initialize@<al>(const struct rasterizer_device_parameters* parameters@<eax>)

    _asm
    {
        push    eax     // parameters
        call    _Hook_rasterizer_device_initialize
        ret
    }
}

ULONG __declspec(naked) __cdecl Trampoline_D3DDevice_Swap(ULONG Flags)
{
    _asm
    {
        // Setup arguments for function call.
        mov     eax, [esp+4]        // Flags

        // Replace instructions we overwrote.
        push    ebx
        push    esi
        mov     esi, global_d3d_device

        // Jump to the original function.
        push    _D3DDevice_Swap+8
        ret
    }
}

ULONG _Hook_D3DDevice_Swap(ULONG Flags)
{
    // Check if the rasterizer globals have been initialized. Swap will be called once during startup
    // to clear any persistent data in the back buffer memory region and we ignore this first call.
    if (Hack_RasterizerTargetsInitialized == false)
        return Trampoline_D3DDevice_Swap(Flags);

    // Check if debug mode is enabled and draw perf counters if so.
    if (Cfg_DebugMode.GetValue<bool>() == true)
    {
        // Draw FPS counter.
        Dbg_DrawPerfCounters();
    }

    // Call the trampoline and let the back buffers rotate.
    ULONG swapCount = Trampoline_D3DDevice_Swap(Flags);

    // Check if triple buffering is enabled and if not bail out.
    if (Hack_TripleBufferingEnabled == false)
        return swapCount;

    // Release references to the back buffer from the previous frame.
    global_d3d_surface_render_primary[0]->Release();
    global_d3d_surface_render_primary[1]->Release();

    // The game stores pointers to the back and front buffer in global_d3d_surface_render_primary[0]/[1] respectively,
    // and will swap them each frame. When triple buffering is enabled this will produce a flickering effect for 
    // anything that uses depth of field effects due to using the incorrect back buffer as an input texture. To work around
    // this we update global_d3d_surface_render_primary[0]/[1] to both point to the current back buffer after Swap completes.
    // The game will still try and swap these pointers later but it doesn't matter since they point to the same underlying
    // memory. This ensures the correct back buffer is always used for depth of field effects.
                
    // Update addresses for back and front buffer globals. Note that D3DDevice_GetBackBuffer will increment
    // the ref count on the back buffer surfaces we retrieve which is why we release the reference to the previously
    // acquired back buffer surfaces above.
    global_d3d_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &global_d3d_surface_render_primary[0]);
    global_d3d_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &global_d3d_surface_render_primary[1]);

    // Update the buffer addresses for all render targets based on the front and back buffer.
    global_d3d_texture_render_primary[0]->Data = global_d3d_surface_render_primary[0]->Data;
    global_d3d_texture_render_primary[1]->Data = global_d3d_surface_render_primary[0]->Data;

    return swapCount;
}

ULONG __declspec(naked) Hook_D3DDevice_Swap(ULONG Flags)
{
    // int __usercall IDirect3DDevice8_Swap@<eax>(int flags@<eax>)

    _asm
    {
        push    eax         // Flags
        call    _Hook_D3DDevice_Swap
        ret
    }
}

void Hook_physical_memory_initialize()
{
    // Zero-initialize physical memory globals.
    memset(&physical_memory_globals, 0, sizeof(physical_memory_globals));

    // Setup physical memory region values.
    physical_memory_globals._start_address = physical_memory_globals.low_stage_address[0] = (void*)_k_physical_memory_base_address;
    physical_memory_globals._end_address = physical_memory_globals.hi_stage_address[0] = (void*)(_k_physical_memory_base_address + Hack_RuntimeDataRegionSize);

    DbgPrint("Hook_physical_memory_initialize size=0x%08x\n", Hack_RuntimeDataRegionSize);

    // Allocate memory for the physical memory region.
    XPhysicalAlloc(Hack_RuntimeDataRegionSize, _k_physical_memory_base_address, 0, PAGE_READWRITE);
}

void Hook_rasterizer_detect_video_mode()
{
    ULONG videoStandard = XGetVideoStandard();
    ULONG videoFlags = XGetVideoFlags();

    if (videoStandard == XC_VIDEO_STANDARD_PAL_I)
        rasterizer_globals._refresh_rate_hz = (videoFlags & XC_VIDEO_FLAGS_PAL_60Hz) != 0 ? 60 : 50;

    rasterizer_globals._letterbox_enabled = (videoFlags & XC_VIDEO_FLAGS_LETTERBOX) != 0;
    rasterizer_globals._widescreen_enabled = (videoFlags & XC_VIDEO_FLAGS_WIDESCREEN) != 0;

    // Support 720p video modes for progressive scan detection.
    rasterizer_globals._progressive_scan_enabled = (videoFlags & (XC_VIDEO_FLAGS_HDTV_480p | XC_VIDEO_FLAGS_HDTV_720p)) != 0;
    rasterizer_globals._progressive_scan_enabled_old = rasterizer_globals._progressive_scan_enabled;
}

void _Hook_rasterizer_init_screen_bounds(word x_off, word y_off, float scale)
{
    // Set default resolution to 640x480.
    float width = 640.0f;
    float height = 480.0f;

    // If widescreen is not enabled on the console force 640x480 or else d3d init will fail.
    if (rasterizer_globals._widescreen_enabled == true)
    {
        // Get console video mode settings.
        DWORD videoFlags = XGetVideoFlags();

        // Mask out video mode flags for resolutions that are disabled in the config file.
        if (Cfg_Enable1080iSupport.GetValue<bool>() == false)
            videoFlags &= ~XC_VIDEO_FLAGS_HDTV_1080i;
        if (Cfg_Enable720pSupport.GetValue<bool>() == false)
            videoFlags &= ~XC_VIDEO_FLAGS_HDTV_720p;

        // Adjust resolution based on current video mode set.
        if ((videoFlags & XC_VIDEO_FLAGS_HDTV_1080i) != 0)
        {
            width = 1920;
            height = 1080;
        }
        else if ((videoFlags & XC_VIDEO_FLAGS_HDTV_720p) != 0)
        {
            width = 1280;
            height = 720;
        }
        else if ((videoFlags & XC_VIDEO_FLAGS_HDTV_480p) != 0)
        {
            width = 720;
        }
    }

    // If the console doesn't have a RAM upgrade force a max resolution of 480p. There's no point in trying 
    // to go any higher because it won't work anyway.
    if (Hack_HasRamUpgrade == false)
    {
        // Check if the resolution was set higher than 480p and if so change it.
        if (width > 720)
        {
            // Force a max resolution of 480p.
            width = 720;
            height = 480;
        }
    }

    rasterizer_globals.screen_bounds.x0 = 0;
    rasterizer_globals.screen_bounds.y0 = 0;
    rasterizer_globals.screen_bounds.x1 = (int)width;
    rasterizer_globals.screen_bounds.y1 = (int)height;

    rasterizer_globals.frame_bounds.x0 = x_off;
    rasterizer_globals.frame_bounds.y0 = y_off;
    rasterizer_globals.frame_bounds.x1 = (int)width - x_off;
    rasterizer_globals.frame_bounds.y1 = (int)height - y_off;
}

void __declspec(naked) Hook_rasterizer_init_screen_bounds(/* word x_off, word y_off, float scale */)
{
    // void __usercall rasterizer_init_screen_bounds(__int16 x_off@<dx>, __int16 y_off@<cx>, float scale@<xmm0>);

    _asm
    {
        // Account for optimized calling convention.
        push    0
        movss   [esp], xmm0     // scale
        push    ecx             // y_off
        push    edx             // x_off
        call    _Hook_rasterizer_init_screen_bounds
        ret
    }
}

bool Hook_rasterizer_primary_targets_initialize()
{
    bool result = true;

    // Get the back buffer, front buffer, and depth buffer surfaces.
    if (Hack_TripleBufferingEnabled == true)
    {
        // We can't reference the front buffer when triple buffering is enabled. Set global_d3d_surface_render_primary surfaces
        // to point to the active back buffer. When D3DDevice_Swap is called Hook_D3DDevice_Swap will handle swapping
        // all surface/texture buffer addresses to point to the active buffer. Internally the game will still flip its accesses
        // to global_d3d_surface_render_primary surfaces every frame, but they point to the same underlying buffer so it doesn't matter.
        global_d3d_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &global_d3d_surface_render_primary[0]);        // Back buffer 0
        global_d3d_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &global_d3d_surface_render_primary[1]);        // Back buffer 0
    }
    else
    {
        // Only using double buffering, grab the front/back buffers as normal.
        global_d3d_device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &global_d3d_surface_render_primary[0]);        // Back buffer
        global_d3d_device->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &global_d3d_surface_render_primary[1]);       // Front buffer
    }

    global_d3d_device->GetDepthStencilSurface(&global_d3d_surface_render_primary_z);

    if (global_d3d_surface_render_primary[0] != nullptr && global_d3d_surface_render_primary[1] != nullptr && global_d3d_surface_render_primary_z != nullptr)
    {
        // Allocate the resource descriptors for texture views of the front/back buffer.
        global_d3d_texture_render_primary[0] = (D3DTexture*)VirtualAlloc(nullptr, sizeof(D3DSurface), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);     // Note: the cast/size mismatch is intentional as it matches the game code
        global_d3d_texture_render_primary[1] = (D3DTexture*)VirtualAlloc(nullptr, sizeof(D3DSurface), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

        // Setup texture views for back/front buffers.
        for (int i = 0; i < 2; i++)
        {
            // Calculate the correct pitch for the texture with tiling enabled. This can be different than the normal pitch
            // value and if set incorrectly will cause a "striping" effect on the back buffer.
            ULONG pitch = D3D::CalcTilePitch(rasterizer_globals.screen_bounds.x1, D3DFMT_LIN_A8R8G8B8);

            XGRAPHICS::XGSetTextureHeader(rasterizer_globals.screen_bounds.x1, rasterizer_globals.screen_bounds.y1, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
                global_d3d_texture_render_primary[i], global_d3d_surface_render_primary[i]->Data, pitch);
        }
    }
    else
        result = false;

    // Create a render target surface for the depth buffer that matches the size and format of the back buffer.
    global_d3d_surface_z_as_target = (D3DSurface*)VirtualAlloc(nullptr, sizeof(D3DSurface), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
    if (global_d3d_surface_z_as_target != nullptr)
    {
        memcpy(global_d3d_surface_z_as_target, global_d3d_surface_render_primary[0], sizeof(D3DSurface));
        global_d3d_surface_z_as_target->Data = global_d3d_surface_render_primary_z->Data;
    }
    else
        result = false;

    // Create two textures for the depth buffer, one in ARGB format and one in ABGR format.
    global_d3d_texture_z_as_target[0] = (D3DTexture*)VirtualAlloc(nullptr, sizeof(D3DSurface), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
    if (global_d3d_texture_z_as_target[0] != nullptr)
    {
        // Calculate the correct pitch for the texture with tiling enabled. This can be different than the normal pitch
        // value and if set incorrectly will cause a "striping" effect on the back buffer.
        ULONG pitch = D3D::CalcTilePitch(rasterizer_globals.screen_bounds.x1, D3DFMT_LIN_A8R8G8B8);

        XGRAPHICS::XGSetTextureHeader(rasterizer_globals.screen_bounds.x1, rasterizer_globals.screen_bounds.y1, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
            global_d3d_texture_z_as_target[0], global_d3d_surface_render_primary_z->Data, pitch);
    }
    else
        result = false;

    global_d3d_texture_z_as_target[1] = (D3DTexture*)VirtualAlloc(nullptr, sizeof(D3DSurface), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
    if (global_d3d_texture_z_as_target[1] != nullptr)
    {
        // Calculate the correct pitch for the texture with tiling enabled. This can be different than the normal pitch
        // value and if set incorrectly will cause a "striping" effect on the back buffer.
        ULONG pitch = D3D::CalcTilePitch(rasterizer_globals.screen_bounds.x1, D3DFMT_LIN_A8B8G8R8);

        XGRAPHICS::XGSetTextureHeader(rasterizer_globals.screen_bounds.x1, rasterizer_globals.screen_bounds.y1, 1, 0, D3DFMT_LIN_A8B8G8R8, 0,
            global_d3d_texture_z_as_target[1], global_d3d_surface_render_primary_z->Data, pitch);
    }
    else
        result = false;

    // Flag that the rasterizer targets have been initialized.
    Hack_RasterizerTargetsInitialized = true;

    return result;
}

bool _Hook_rasterizer_allocate_and_create_render_target(int rasterizer_target, int width, int height, bool unk, bool z_surface)
{
    // For now we only need to adjust the resolution of the texaccum target. In the future if we need to adjust the size
    // of other rasterizer render targets it would be better to hook rasterizer_targets_initialize, it's just a bit more work
    // to reimplement than I want to do right now...

    // Check the rasterizer target for the texaccum target.
    if (rasterizer_target == _rasterizer_target_texaccum)
    {
        // Adjust the width/height to match the screen resolution set.
        width = rasterizer_globals.screen_bounds.x1;
        height = rasterizer_globals.screen_bounds.y1;
    }

    DbgPrint("_Hook_rasterizer_allocate_and_create_render_target: target=%d width=%d height=%d unk=%d z_surface=%d\n",
        rasterizer_target, width, height, unk, z_surface);

    // If the console has 128MB of RAM allocate the texaccum target in the upper 64MB region.
    void* renderTargetBuffer;
    if (rasterizer_target == _rasterizer_target_texaccum && Hack_HasRamUpgrade == true)
    {
        // Calculate the allocation size.
        int allocationSize = ((((width + (D3DTEXTURE_PITCH_ALIGNMENT - 1)) & ~(D3DTEXTURE_PITCH_ALIGNMENT - 1)) * height * 4) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

        // Allocate a new buffer in the upper 64MB.
        renderTargetBuffer = Hack_PhysicalMemoryAlloc(PhysMemRegion_RasterizerTexaccumTarget, allocationSize, PAGE_READWRITE | PAGE_WRITECOMBINE);
        renderTargetBuffer = (void*)(_k_physical_memory_address_mask | (unsigned int)renderTargetBuffer);
    }
    else
    {
        // Calculate the allocation size rounded up to the nearest page size.
        int allocationSize = ((((width + (D3DTEXTURE_PITCH_ALIGNMENT - 1)) & ~(D3DTEXTURE_PITCH_ALIGNMENT - 1)) * height * 4) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

        // Allocate memory from the game's self-managed memory pool.
        renderTargetBuffer = (unsigned char*)physical_memory_globals.hi_stage_address[physical_memory_globals.current_stage] - allocationSize;
        if (renderTargetBuffer >= physical_memory_globals.low_stage_address[physical_memory_globals.current_stage])
        {
            physical_memory_globals.hi_stage_address[physical_memory_globals.current_stage] = renderTargetBuffer;

            // Mark the allocated memory as RW write-combine memory.
            renderTargetBuffer = (void*)(_k_physical_memory_address_mask | (unsigned int)renderTargetBuffer);
            XPhysicalProtect(renderTargetBuffer, allocationSize, PAGE_READWRITE | PAGE_WRITECOMBINE);
        }
    }

    if (renderTargetBuffer == nullptr)
        DBG_BREAKPOINT();

    // Setup the render target structure.
    bool result = _rasterizer_create_render_target(rasterizer_target, _rasterizer_target_type_static, width, height, z_surface, /* linear */ true, true, renderTargetBuffer);
    if (result == false)
    {
        DbgPrint("!!! ERROR: target=%d width=%d height=%d unk=%d z_surface=%d buffer=0x%08x\n",
            rasterizer_target, width, height, unk, z_surface, renderTargetBuffer);
        DBG_BREAKPOINT();
    }

    return result;
}

bool __declspec(naked) Hook_rasterizer_allocate_and_create_render_target(int rasterizer_target, int width, /* int height, */ bool unk, bool z_surface)
{
    // bool __usercall __rasterizer_allocate_and_create_render_target@<al>(int rasterizer_target, int width, int height@<ebx>, bool, bool z_surface)

    _asm
    {
        push    [esp+0x10]      // z_surface
        push    [esp+4+0xC]     // unk
        push    ebx             // height
        push    [esp+0xC+8]     // width
        push    [esp+0x10+4]    // rasterizer_target
        call    _Hook_rasterizer_allocate_and_create_render_target
        ret     4 * 4
    }
}

bool _Hook_rasterizer_get_render_target_resolution(int rasterizer_target, int* resolution_x, int* resolution_y)
{
    // Check for special case targets.
    if (_rasterizer_render_targets[rasterizer_target].data == nullptr || _rasterizer_render_targets[rasterizer_target].pending_deallocation == true)
    {
        // Render target 0: front buffer
        // Render target 3: back buffer
        // Render target 24: ?
        if (rasterizer_target == 0 || rasterizer_target == 3 || rasterizer_target == 24)
        {
            // Use video resolution for render target size.
            *resolution_x = rasterizer_globals.screen_bounds.x1;
            *resolution_y = rasterizer_globals.screen_bounds.y1;

            return true;
        }
        else
        {
            *resolution_x = 0;
            *resolution_y = 0;

            return false;
        }
    }

    // Use resolution specified by the render target descriptor.
    *resolution_x = _rasterizer_render_targets[rasterizer_target].width;
    *resolution_y = _rasterizer_render_targets[rasterizer_target].height;

    return true;
}

bool __declspec(naked) Hook_rasterizer_get_render_target_resolution(/* int rasterizer_target, int* resolution_x, int* resolution_y */)
{
    // bool __usercall rasterizer_get_render_target_resolution@<al>(int rasterizer_target@<eax>, int *resolution_x@<edx>, int *resolution_y@<ecx>)

    _asm
    {
        push    ecx             // resolution_y
        push    edx             // resolution_x
        push    eax             // rasterizer_target
        call    _Hook_rasterizer_get_render_target_resolution
        ret
    }
}

void __declspec(naked) Hook_create_render_target_18_helper()
{
    _asm
    {
        // Use screen resolution for creating render target 18 (memory is stolen from the standard texture cache).
        lea     ecx, rasterizer_globals
        movsx   ax, word ptr [ecx+2+4]      // rasterizer_globals.screen_bounds.y1
        mov     [esp+4], eax                // width = rasterizer_globals.screen_bounds.y1
        movsx   ax, word ptr [ecx+2+6]      // rasterizer_globals.screen_bounds.x1
        mov     [esp+8], eax                // height = rasterizer_globals.screen_bounds.x1

        // Replace instructions we overwrote.
#ifdef H2_1_0
        mov     eax, VERSION_SPECIFIC_ADDR(0x485898)
        mov     eax, [eax]
        push    ebp
        push    VERSION_SPECIFIC_ADDR(0x0001DC46)
        ret
#elif H2_1_5
        mov     eax, VERSION_SPECIFIC_ADDR(0x4920B8)
        mov     eax, [eax]
        push    ebp
        push    VERSION_SPECIFIC_ADDR(0x0001DC26)
        ret
#endif
    }
}

bool Hook_rasterizer_should_render_screen_effect()
{
    // Compare viewport bounds against display resolution.
    if (global_window_parameters__camera.viewport_bounds.x0 == 0 && 
        global_window_parameters__camera.viewport_bounds.y0 == 0 &&
        global_window_parameters__camera.viewport_bounds.x1 == rasterizer_globals.screen_bounds.x1 && 
        global_window_parameters__camera.viewport_bounds.y1 == rasterizer_globals.screen_bounds.y1)
    {
        if (render_globals__player_window_count <= 1)
            return true;
    }

    return false;
}

bool Hook_rasterizer_fog_composite_get_register(int register_index, real_rectangle_2d* bounds, real_vector_2d* location, real_vector_4d* output, void* unk)
{
    // Call the original function.
    bool result = rasterizer_fog_composite_get_register(register_index, bounds, location, output, unk);

    // Only modify the output values for vertex element 4 (TEXCOORD3).
    if (register_index == 4)
    {
        // Check the output vector and correct texcoord positions.
        if (output->x != 0.f)
            output->x = 320.0f;
        if (output->y != 0.f)
            output->y = 240.0f;
    }

    return result;
}

void Hook_interface_splitscreen_render()
{
    rectangle_2d rect;

    // Make sure there's at least two or more player windows.
    if (render_globals__player_window_count <= 1)
        return;

    // Calculate the half width/height bounds.
    int halfWidth = rasterizer_globals.screen_bounds.x1 / 2;
    int halfHeight = rasterizer_globals.screen_bounds.y1 / 2;

    // Check the split screen mode and handle accordingly.
    if (render_globals___screen_split_mode == 1)
    {
        // Draw vertical split.
        rect.x0 = 0;
        rect.y0 = halfHeight - 1;
        rect.x1 = rasterizer_globals.screen_bounds.x1;
        rect.y1 = halfHeight + 1;
        draw_quad(&rect, 0xFF000000);

        // Check if there's more than 2 players.
        if (render_globals__player_window_count > 2)
        {
            // Draw horizontal split.
            rect.x0 = halfWidth - 1;
            rect.y0 = render_globals__player_window_count == 3 ? halfHeight : 0;
            rect.x1 = halfWidth + 1;
            rect.y1 = rasterizer_globals.screen_bounds.y1;
            draw_quad(&rect, 0xFF000000);
        }
    }
    else
    {
        // Draw vertical split.
        rect.x0 = halfWidth - 1;
        rect.y0 = 0;
        rect.x1 = halfWidth + 1;
        rect.y1 = rasterizer_globals.screen_bounds.y1;
        draw_quad(&rect, 0xFF000000);

        // Check if there's more than 2 players.
        if (render_globals__player_window_count > 2)
        {
            // Draw horizontal split.
            rect.x0 = render_globals__player_window_count == 3 ? halfWidth : 0;
            rect.y0 = halfHeight - 1;
            rect.x1 = rasterizer_globals.screen_bounds.x1;
            rect.y1 = halfHeight + 1;
            draw_quad(&rect, 0xFF000000);
        }
    }
}

int _Hook__renderer_setup_player_windows()
{
    // Check if widescreen mode is enabled.
    if (rasterizer_globals._widescreen_enabled == false)
    {
        // Use default setting (horizontal split).
        return 1;
    }

    // Use the value specified by the config file.
    return clamp(Cfg_SplitScreenFavor.GetValue<int>(), 1, 2);
}

void __declspec(naked) Hook__renderer_setup_player_windows()
{
    _asm
    {
        // Determine which split screen mode to use.
        call    _Hook__renderer_setup_player_windows
        mov     [esp+0x10], eax

        // Jump back to the original function.
#ifdef H2_1_0
        push    VERSION_SPECIFIC_ADDR(0x00223585)
        ret
#elif H2_1_5
        push    VERSION_SPECIFIC_ADDR(0x00229F95)
        ret
#endif
    }
}

void Hook_geometry_cache_open(int unknown)
{
    // If the geometry cache globals haven't been initialized bail out.
    if (xbox_geometry_cache_globals.cache == nullptr)
        return;

    unsigned int cacheSize = 0;

    // Set the size of the geometry cache based on the resolution set, 720p or lower gets more memory.
    if (rasterizer_globals.screen_bounds.x1 == 1920)
        cacheSize = EXPANDED_GEOMETRY_CACHE_SIZE;
    else
        cacheSize = EXPANDED_GEOMETRY_CACHE_SIZE_720;

    // Round the cache size up the nearest page size and allocate the region.
    cacheSize = (cacheSize + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
    void* cacheAddress = Hack_PhysicalMemoryAlloc(PhysMemRegion_GeometryCache, cacheSize, PAGE_READWRITE);

    xbox_geometry_cache_globals.cache_base_address = (void*)(_k_physical_memory_address_mask | (unsigned int)cacheAddress);
    xbox_geometry_cache_globals.cache_size = cacheSize / PAGE_SIZE << 12;

    // Resize the lruv cache.
    lruv_resize(xbox_geometry_cache_globals.cache, cacheSize / PAGE_SIZE);
    *(unsigned int*)((BYTE*)xbox_geometry_cache_globals.cache + 0x2C) = 2;
}

void Hook_geometry_cache_close()
{
    // Run original function logic.
    if (xbox_geometry_cache_globals.cache != nullptr)
    {
        lruv_resize(xbox_geometry_cache_globals.cache, 0);
        xbox_geometry_cache_globals.cache_base_address = nullptr;
    }

    // Free the allocation.
    Hack_PhysicalMemoryFree(PhysMemRegion_GeometryCache);
}

void Hook_texture_cache_open()
{
    int cacheSize = 0;

    // Set the size of the texture cache based on the resolution set, 720p or lower gets more memory.
    if (rasterizer_globals.screen_bounds.x1 == 1920)
        cacheSize = EXPANDED_TEXTURE_CACHE_SIZE;
    else
        cacheSize = EXPANDED_TEXTURE_CACHE_SIZE_720;

    // Round the cache size up the nearest page size and allocate the region.
    cacheSize = (cacheSize + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
    void* cacheAddress = Hack_PhysicalMemoryAlloc(PhysMemRegion_TextureCache, cacheSize, PAGE_READWRITE | PAGE_WRITECOMBINE);

    xbox_texture_cache_globals.standard_cache_base_address = (void*)(_k_physical_memory_address_mask | (unsigned int)cacheAddress);

    // Initialize cache tables.
    lruv_resize(xbox_texture_cache_globals.standard_cache, cacheSize / PAGE_SIZE);
    data_make_valid(xbox_texture_cache_globals.textures);
    data_make_valid(xbox_texture_cache_globals.predicted_textures);
}

void Hook_texture_cache_close()
{
    xbox_texture_cache_globals.unk4 = true;

    // Push any remaining d3d commands to the GPU and flush cache tables.
    texture_cache_flush();
    data_make_invalid(xbox_texture_cache_globals.textures);
    data_make_invalid(xbox_texture_cache_globals.predicted_textures);

    lruv_resize(xbox_texture_cache_globals.standard_cache, 0);
    xbox_texture_cache_globals.standard_cache_base_address = nullptr;
    xbox_texture_cache_globals.low_detail_cache_base_address = nullptr;

    // Free the allocation.
    Hack_PhysicalMemoryFree(PhysMemRegion_TextureCache);
}

void* Hack_PhysicalMemoryAlloc(int regionIndex, int size, int protect)
{
    // Get the region descriptor and update the region size if one was specified.
    PhysicalMemoryRegionDescriptor* pRegionDesc = &Hack_PhysicalMemoryRegionInfoTable[regionIndex];
    if (size != 0)
        pRegionDesc->Size = (size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

    // If the region had already been allocated break in.
    if (pRegionDesc->BaseAddress != nullptr)
        DBG_BREAKPOINT();

    // Allocate physical contiguous memory. The memory manager will search for an available memory region
    // starting at the end of RAM and working towards the beginning. If no contiguous region is found it
    // will relocate virtual allocations to satisfy the request.
    pRegionDesc->BaseAddress = MmAllocateContiguousMemoryEx(pRegionDesc->Size, 0, 0xFFFFFFFF, PAGE_SIZE, protect);
    if (pRegionDesc->BaseAddress == nullptr)
    {
        DbgPrint("Hack_PhysicalMemoryAlloc: failed to allocate region '%s' for size 0x%08x!\n", pRegionDesc->Name, pRegionDesc->Name);
        DBG_BREAKPOINT();
    }

    // Print allocation info.
    DbgPrint("physical_memory_malloc %s %ld at 0x%08x\n", pRegionDesc->Name, pRegionDesc->Size, pRegionDesc->BaseAddress);

    return pRegionDesc->BaseAddress;
}

void Hack_PhysicalMemoryFree(int regionIndex)
{
    // Get the region descriptor and make sure the region has been allocated.
    PhysicalMemoryRegionDescriptor* pRegionDesc = &Hack_PhysicalMemoryRegionInfoTable[regionIndex];
    if (pRegionDesc->BaseAddress == nullptr)
        return;

    DbgPrint("physical_memory_free %s\n", pRegionDesc->Name);

    // Free the allocation.
    MmFreeContiguousMemory(pRegionDesc->BaseAddress);
    pRegionDesc->BaseAddress = nullptr;
}

void __declspec(naked) Hook_bink_playback_start(const char* file_path, unsigned int flags)
{
    _asm
    {
        // Mask in the flag to stretch video to screen resolution.
        or      [esp+8], 0x10           // flags |= 0x10

        // Replace instructions we overwrote.
        sub     esp, 0x40
        xor     eax, eax
        mov     [esp+0x20], eax

        // Jump back to the original function.
        push    bink_playback_start+9
        ret
    }
}

bool Hook__should_draw_player_hud()
{
    return false;
}

void Hack_ColdRebootConsole()
{
    // If a custom fan speed was used change it back to normal.
    if (Cfg_OverrideFanSpeed.GetValue<bool>() == true)
    {
        // Set the fan speed back to stock.
        HalWriteSMBusValue(
            0x20,               // SMC_SLAVE_ADDRESS
            6,                  // SMC_COMMAND_REQUEST_FAN_SPEED
            FALSE,
            10                  // Fan speed
        );

        // Give the SMC a chance to process the message, if not it can panic.
        KeStallExecutionProcessor(100*1000);

        // Set SMC fan mode to use stock speed (this doesn't seem to work if the override speed is still set).
        HalWriteSMBusValue(
            0x20,               // SMC_SLAVE_ADDRESS
            5,                  // SMC_COMMAND_FAN_OVERRIDE
            FALSE,
            0                   // SMC_FAN_OVERRIDE_DEFAULT
        );
        KeStallExecutionProcessor(100 * 1000);
    }

    // Check IRQL level to see if we can use HalWriteSMBusValue and if so have the SMC do a full reset for us.
    if (KeGetCurrentIrql() < 2)     // DISPATCH_LEVEL
    {
        // Have the SMC do a full reset.
        HalWriteSMBusValue(
            0x20,               // SMC_SLAVE_ADDRESS
            2,                  // SMC_COMMAND_RESET
            FALSE,
            1                   // SMC_RESET_ASSERT_RESET
        );
    }

    // Perform full PCI reset.
    __outbyte(
        0xCF9,                  // RESET_CONTROL_REGISTER
        0xE                     // RESET_CONTROL_FULL_RESET | RESET_CONTROL_RESET_CPU | RESET_CONTROL_SYSTEM_RESET
    );

halt:
    // Halt the CPU and wait for the reboot, thanks for playing...
    _disable();         // cli
    __halt();           // hlt
    goto halt;
}
