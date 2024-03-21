; ////////////////////////////////////////////////////////
; ////////////////// Preprocessor Stuff //////////////////
; ////////////////////////////////////////////////////////

BITS 32

%define ExecutableBaseAddress			00010000h			; Base address of the executable
%define HacksSegmentAddress				00586000h			; Virtual address of the .hacks segment
%define HacksSegmentOffset				004a3000h			; File offset of the .hacks segment
%define HacksSegmentSize				00002000h			; Size of the .hacks segment

%include "Utilities_h.asm"
%include "DirectX_h.asm"

%macro PHYS_MEM_REGION_STR 2
	_%1:
		db %2,0
		align 4, db 0
	
	HACK_DATA %1			
%endmacro

%macro PHYS_MEM_REGION 2
	dd		%1
	dd		%2
	dd		0
%endmacro

struc RECT
	.upper		resw	1
	.left		resw	1
	.lower		resw	1
	.right		resw	1
endstruc
ASSERT_STRUCT_SIZE RECT, 8

; Memory addresses for game functions and data:
%define game_main									00012190h
%define _crc32_calculate_opt						00163BA0h	; void _crc32_calculate(int* checksum, unsigned char* buffer<eax>, int size<edi>)

%define _rasterizer_init_screen_bounds				00016A30h
%define rasterizer_primary_targets_initialize		00014600h
%define _rasterizer_create_render_target			0001DF20h	; bool _rasterizer_create_render_target(int rasterizer_target<eax>, int type, int width, int height, int z_surface, int linear, int unk, void* buffer)
%define _rasterizer_alloc_and_create_render_target	0001DB10h	; bool _rasterizer_alloc_and_create_render_target(int rasterizer_target, int width, int height<ebx>, int unk, int z_surface)
%define rasterizer_targets_initialize				0001D770h
%define _rasterizer_detect_video_mode				000123B0h

%define _renderer_draw_color_rect					0013C050h	; void _renderer_draw_color_rect(RECT* pRect<ecx>, int color<eax>)
%define _rasterizer_draw_string						0001FA50h	; void _rasterizer_draw_string(char* string<ecx>, RECT* bounds, RECT* unk1, int unk2, int unk3, float unk4, int unk5, int unk6)
%define _draw_string_set_primary_color				0013EC70h	; void _draw_string_set_primary_color(vector3* color<eax>)
%define _draw_string_set_shadow_color				0013ED50h	; void _draw_string_set_shadow_color(vector3* color<eax>)

%define _g_rasterizer_draw_string_font				004E73A0h	; DWORD
%define _g_rasterizer_draw_string_flags				004E73A4h	; DWORD
%define _g_rasterizer_draw_string_style				004E73A8h	; DWORD
%define _g_rasterizer_draw_string_justification		004E73ACh	; DWORD

%define _draw_split_screen_window_bars				0013E635h

%define _should_draw_player_hud						0013939Bh

%define _initialize_standard_texture_cache			0012C1E0h

%define rasterizer_globals_screen_bounds_y0			00485A8Ah	; WORD
%define rasterizer_globals_screen_bounds_x0			00485A8Ch	; WORD
%define rasterizer_globals_screen_bounds_y1			00485A8Eh	; WORD
%define rasterizer_globals_screen_bounds_x1			00485A90h	; WORD

%define _rasterizer_globals_frame_bounds_y0			00485A92h	; WORD
%define _rasterizer_globals_frame_bounds_x0			00485A94h	; WORD
%define _rasterizer_globals_frame_bounds_y1			00485A96h	; WORD
%define _rasterizer_globals_frame_bounds_x1			00485A98h	; WORD

%define _g_refresh_rate_hz							00485AC0h	; WORD
%define _g_widescreen_enabled						00485AC2h	; BYTE
%define _g_progressive_scan_enabled					00485AC6h	; BYTE

%define g_camera_fov_scale							0047136Ch	; float

%define global_d3d_device							005093B0h	; IDirect3DDevice8*

%define global_d3d_texture_render_primary_0			00509354h	; IDirect3DTexture8*
%define global_d3d_texture_render_primary_1			00509358h	; IDirect3DTexture8*
%define global_d3d_surface_render_primary_0			0050935Ch	; IDirect3DSurface8*
%define global_d3d_surface_render_primary_1			00509360h	; IDirect3DSurface8*
%define global_d3d_surface_render_primary_z			00509364h	; IDirect3DSurface8*
%define global_d3d_texture_z_as_target_0			00509368h	; IDirect3DTexture8*
%define global_d3d_texture_z_as_target_1			0050936Ch	; IDirect3DTexture8*

%define _g_swap_count_array_index					004E6400h	; WORD
%define _g_swap_count_array							004E6402h	; WORD[15]
%define physical_memory_globals_current_stage		004E6420h	; DWORD
%define physical_memory_globals_low_stage_address	004E642Ch	; DWORD
%define physical_memory_globals_hi_stage_address	004E6440h	; DWORD

%define render_globals_player_window_count			004BA04Ch	; DWORD

; DirectX function addresses:
%define IDirect3D8_CreateDevice						003F5240h
%define IDirect3DDevice8_Swap						003F9D00h	; int IDirect3DDevice8_Swap(int flags<eax>)
%define IDirect3DDevice8_SetTile					003F8690h	; void IDirect3DDevice8_SetTile(int index<eax> D3DTILE* tile<ecx>)
%define D3DResource_AddRef							003FB600h	; int D3DResource_AddRef(D3DResource* resource)
%define D3DResource_Release							003FB640h	; int D3DResource_Release(D3DResource* resource)

%define D3D_CalcTilePitch							003FD820h	; int D3D::CalcTilePitch(int width, D3DFORMAT format<eax>)

; Internal d3d device pointer:
%define D3D_g_pDevice								00407488h

%define DbgPrint									0036D1E2h
%define XapiBootToDash								002D1AD8h
%define XGetVideoFlags								00330339h
%define CreateFileA									002D2750h
%define ReadFile									002D2241h
%define GetFileSize									002D2715h
%define CloseHandle									002D1F3Ah

%define malloc										00322E16h	; __cdecl
%define free										00324C91h	; __cdecl
%define lstrlenA									002D1469h
%define atoi64										00321740h	; __cdecl
%define snwprintf									00320725h	; __cdecl

; Utility functions to compile in:
UTIL_INCLUDE lstrcpyA
UTIL_INCLUDE atof

; D3D functions to compile in:
D3D_INCLUDE IDirect3DDevice8_GetBackBuffer
D3D_INCLUDE IDirect3DDevice8_GetTile

; Memory addresses of our hack functions and data.
HACK_FUNCTION Hack_InitHacks
HACK_FUNCTION Hack_InstallHook
HACK_FUNCTION Hook_IDirect3D8_CreateDevice
HACK_FUNCTION Hook_IDirect3DDevice8_Swap
HACK_FUNCTION Hook_IDirect3DDevice8_Swap_reentry
HACK_FUNCTION Hook_physical_memory_allocate
HACK_FUNCTION Hook__rasterizer_init_screen_bounds
HACK_FUNCTION Hook_rasterizer_preinitialize
HACK_FUNCTION Hook_rasterizer_device_initialize
HACK_FUNCTION Hook_rasterizer_primary_targets_initialize
HACK_FUNCTION Hook_rasterizer_primary_targets_initialize_reentry
HACK_FUNCTION Hook__rasterizer_alloc_and_create_render_target
HACK_FUNCTION Hook__rasterizer_create_render_target
HACK_FUNCTION Hook_rasterizer_targets_initialize
HACK_FUNCTION Hook_rasterizer_get_render_target_resolution
HACK_FUNCTION Hook_create_render_target_helper
HACK_FUNCTION Hook_should_render_screen_effect
HACK_FUNCTION Hook__fog_build_vertex_element
HACK_FUNCTION Hook__draw_split_screen_window_bars
HACK_FUNCTION Hook__renderer_setup_player_windows
HACK_FUNCTION Hook__should_draw_player_hud

HACK_FUNCTION Hook__initialize_geometry_cache
HACK_FUNCTION Hook__geometry_cache_globals_cleanup
HACK_FUNCTION Hook__initialize_standard_texture_cache
HACK_FUNCTION Hook__texture_cache_globals_cleanup
HACK_FUNCTION Hack_PhysicalMemoryAlloc
HACK_FUNCTION Hack_PhysicalMemoryFree
HACK_FUNCTION Hack_ColdRebootConsole

HACK_FUNCTION HalReadWritePCISpace
HACK_FUNCTION HalReturnToFirmware
HACK_FUNCTION HalWriteSMBusValue
HACK_FUNCTION KeGetCurrentIrql
HACK_FUNCTION KeStallExecutionProcessor
HACK_FUNCTION MmAllocateContiguousMemoryEx
HACK_FUNCTION MmFreeContiguousMemory

HACK_FUNCTION crc32_calculate_stdcall
%define _crc32_calculate crc32_calculate_stdcall

HACK_DATA Util_KernelImports
HACK_DATA Hack_StartupMessage
HACK_DATA Hack_CreateDeviceFormatString
HACK_DATA Hack_PhysicalMemoryMallocInfoString
HACK_DATA Hack_PhysicalMemoryRegionInfoTable

HACK_DATA Hack_RasterizerTargetsInitialized		; DWORD

HACK_DATA Hack_HasRAMUpgrade					; BYTE
HACK_DATA Hack_TripleBufferingEnabled			; BYTE
HACK_DATA Hack_DisableZCompress					; BYTE

HACK_DATA Hack_RuntimeDataRegionSize			; DWORD
HACK_DATA Hack_RuntimeDataRegionEndAddress		; DWORD

; Config.asm:
HACK_FUNCTION Cfg_ParseConfigFile
HACK_DATA Cfg_ConfigFileOptionTable
HACK_DATA Cfg_ConfigFilePath


HACK_DATA Cfg_Enable1080iSupport
HACK_DATA Cfg_Enable720pSupport
HACK_DATA Cfg_DisableAnamorphicScaling
HACK_DATA Cfg_DisableAtmosphericFog
HACK_DATA Cfg_FieldOfView
HACK_DATA Cfg_SplitScreenFavor
HACK_DATA Cfg_DisableHud
HACK_DATA Cfg_DebugMode
HACK_DATA Cfg_OverclockGPU
HACK_DATA Cfg_GPUOverclockStep
HACK_DATA Cfg_OverrideFanSpeed
HACK_DATA Cfg_FanSpeedPercent

; Region indices for Hack_PhysicalMemoryRegionInfoTable:
%define PHYS_MEM_REGION_GEOMETRY_CACHE								0
%define PHYS_MEM_REGION_TEXTURE_CACHE								1
%define PHYS_MEM_REGION_RASTERIZER_TEXACCUM_TARGET					2


; Compilation options:
%define RUNTIME_DATA_REGION_SIZE_ADJUST_128MB	1024*1024*25		; 25MB size reduction for 128MB consoles
%define RUNTIME_DATA_REGION_SIZE_ADJUST_64MB	1024*1024*1			; 1MB size reduction for 64MB consoles
%define EXPANDED_TEXTURE_CACHE_SIZE				1024*1024*25		; 20MB
%define EXPANDED_GEOMETRY_CACHE_SIZE			1024*1024*15		; 12MB (SP = 6.5MB, MP = 7MB)

;---------------------------------------------------------
; Patch the XBE header flags to remove the XINIT_LIMIT_DEVKIT_MEMORY to allow use of upper 64MB of RAM
;---------------------------------------------------------
dd		124h
dd		(_xbe_header_flags_end - _xbe_header_flags_start)
_xbe_header_flags_start:

		dd		80000001h

_xbe_header_flags_end:

;---------------------------------------------------------
; Hook main so we can setup our hooks
;---------------------------------------------------------
dd		002D0AD0h - ExecutableBaseAddress
dd		(_main_hook_end - _main_hook_start)
_main_hook_start:

		; Jump to our init code cave.
		push	Hack_InitHacks
		ret

_main_hook_end:

;---------------------------------------------------------
; _rasterizer_detect_video_mode -> Updates for HD video modes
;---------------------------------------------------------
dd		000123E5h - ExecutableBaseAddress
dd		_detect_video_mode_end - _detect_video_mode_start
_detect_video_mode_start:

		; Set progressive mode for 480p and 720p resolutions.
		and		eax, 8 | 2			; bProgressiveScan = videoFlags & (XC_VIDEO_FLAGS_HDTV_480p | XC_VIDEO_FLAGS_HDTV_720p)

_detect_video_mode_end:

;---------------------------------------------------------
; Hook _fog_build_vertex_element to use correct texture coordinates
;---------------------------------------------------------
dd		00025E08h - ExecutableBaseAddress
dd		(_render_patchy_fog_hook_end - _render_patchy_fog_hook_start)
_render_patchy_fog_hook_start:

		push	Hook__fog_build_vertex_element

_render_patchy_fog_hook_end:



;---------------------------------------------------------
; .hacks code segment
;---------------------------------------------------------
dd			HacksSegmentOffset
dd			(_hacks_code_end - _hacks_code_start)
_hacks_code_start:

	;---------------------------------------------------------
	; void HacksInit() -> Hook game code and perform any initialization
	;---------------------------------------------------------
_Hack_InitHacks:

		; Print startup message.
		push	Hack_StartupMessage
		mov		eax, DbgPrint
		call	eax
		
		;INT3

		; Parse the config file.
		call	_Cfg_ParseConfigFile

		; Resolve kernel imports.
		call	_Util_ResolveKernelImports
		
		; Check if we should overclock the GPU.
		cmp		byte [Cfg_OverclockGPU], 1
		jnz		.check_memory
		
			; Check that the fan speed override has been set and force it to max speed if not.
			cmp		byte [Cfg_OverrideFanSpeed], 1
			jz		.set_fan_speed
			
				; Force fan speed to max because the user is an idiot and didn't read the big fat
				; warnings about cooking the GPU on stock fan speeds. If they choose to enabled the
				; override and not set the fan speed high enough then RIP to their GPU.
				mov		dword [Cfg_FanSpeedPercent], 100
		
.set_fan_speed:
		
			; Set the fan speed first.
			push	dword [Cfg_FanSpeedPercent]
			call	_Util_SetFanSpeed
			
			; Update GPU clock configuration.
			push	dword [Cfg_GPUOverclockStep]
			call	_Util_OverclockGPU
		
.check_memory:
		
		; Check if the console has 128MB of RAM.
		call	_Util_GetMemoryCapacity
		cmp		eax, 128
		jnz		.low_memory_install_hooks
		
			; Flag the console has 128MB of RAM.
			mov		byte [Hack_HasRAMUpgrade], 1
			
			; Force enable triple buffering, no reason not to...
			mov		byte [Hack_TripleBufferingEnabled], 1
			
			; Adjust the size of the runtime data region since we'll be moving the texture and geometry cache out.
			sub		dword [Hack_RuntimeDataRegionSize], RUNTIME_DATA_REGION_SIZE_ADJUST_128MB
			sub		dword [Hack_RuntimeDataRegionEndAddress], RUNTIME_DATA_REGION_SIZE_ADJUST_128MB
			
			; Hook physical memory and cache initialization functions.
			HOOK_FUNCTION _rasterizer_alloc_and_create_render_target, Hook__rasterizer_alloc_and_create_render_target
			HOOK_FUNCTION 0012DA30h, Hook__initialize_geometry_cache
			HOOK_FUNCTION 0012DAD0h, Hook__geometry_cache_globals_cleanup
			HOOK_FUNCTION _initialize_standard_texture_cache, Hook__initialize_standard_texture_cache
			HOOK_FUNCTION 0012C290h, Hook__texture_cache_globals_cleanup
			
			; Patch the max usable PFN for contiguous allocations.
			call	_Hack_PatchMaxPFN
			
			jmp		.install_hooks
			
.low_memory_install_hooks:

		; Install hooks specific to consoles with no RAM upgrade.
		HOOK_FUNCTION _rasterizer_create_render_target, Hook__rasterizer_create_render_target
		HOOK_FUNCTION rasterizer_targets_initialize, Hook_rasterizer_targets_initialize
		
		; Adjust the size of the runtime data region to account for increased front/back buffers.
		sub		dword [Hack_RuntimeDataRegionSize], RUNTIME_DATA_REGION_SIZE_ADJUST_64MB
		sub		dword [Hack_RuntimeDataRegionEndAddress], RUNTIME_DATA_REGION_SIZE_ADJUST_64MB

.install_hooks:
		
		; Install hooks.
		HOOK_FUNCTION 0012B41Dh, Hook_physical_memory_allocate
		HOOK_FUNCTION IDirect3D8_CreateDevice, Hook_IDirect3D8_CreateDevice
		HOOK_FUNCTION _rasterizer_init_screen_bounds, Hook__rasterizer_init_screen_bounds
		HOOK_FUNCTION 00012517h, Hook_rasterizer_preinitialize
		HOOK_FUNCTION 000129AAh, Hook_rasterizer_device_initialize
		HOOK_FUNCTION rasterizer_primary_targets_initialize, Hook_rasterizer_primary_targets_initialize
		HOOK_FUNCTION 0001DDB9h, Hook_rasterizer_get_render_target_resolution
		HOOK_FUNCTION 0001DC40h, Hook_create_render_target_helper
		HOOK_FUNCTION 00022726h, Hook_should_render_screen_effect
		HOOK_FUNCTION _draw_split_screen_window_bars, Hook__draw_split_screen_window_bars
		HOOK_FUNCTION 00223564h, Hook__renderer_setup_player_windows
		HOOK_FUNCTION IDirect3DDevice8_Swap, Hook_IDirect3DDevice8_Swap
		
		; Hook HalReturnToFirmware so that any attempt to quit the game results in a cold reboot of the console.
		; Due to how we hot patch the kernel (in 128MB mode) the console is basically "hosed" after running the game. Any attempt
		; to run another executable, return to dash, IRG, etc, will result in graphical artifacting and the console freezing.
		; If the console doesn't have extra RAM but the user overclocks the GPU we want to reset that as well.
		HOOK_FUNCTION dword [HalReturnToFirmware], Hack_ColdRebootConsole
		
		; Calculate fov scale.
		movss	xmm0, dword [Cfg_FieldOfView]
		mov		eax, 70
		cvtsi2ss	xmm1, eax
		divss	xmm0, xmm1
		movss	dword [g_camera_fov_scale], xmm0
		
		; Check if we should disable the HUD.
		cmp		dword [Cfg_DisableHud], 1
		jnz		.disable_fog
		
			; Hook the should draw hud function and always disable it.
			HOOK_FUNCTION _should_draw_player_hud, Hook__should_draw_player_hud
			
.disable_fog:

		; Check if we should disable atmospheric fog for more perf.
		cmp		dword [Cfg_DisableAtmosphericFog], 0
		jz		.disable_anamorphic_scaling
		
			; Hook the rendering function for atmospheric fog to a stub that just returns.
			HOOK_FUNCTION 00025D50h, 003796F0h
			
.disable_anamorphic_scaling:

		; Check if we should disable anamorphic scaling.
		cmp		dword [Cfg_DisableAnamorphicScaling], 0
		jz		.run_main
		
			; Change the anamorphic scaling factor to 1.0
			push	__?float32?__(1.0)
			push	0045DFDCh
			call	_Util_WriteDword

.run_main:
		
		; Jump to game entry point.
		push	0
		push	0
		push	0
		mov		eax, game_main
		call	eax
		add		esp, 0Ch
		
		; Boot to dashboard.
		push	0
		push	1
		push	1
		mov		eax, XapiBootToDash
		call	eax
		xor		eax, eax
		retn 4
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_IDirect3D8_CreateDevice -> Print present parameters
	;---------------------------------------------------------
_Hook_IDirect3D8_CreateDevice:

		%define StackSize			8h
		%define StackStart			0h
		%define pPresentParams		4h
		
		; Setup the stack frame.
		sub		esp, StackStart
		push	ecx					; ppReturnedDevicePtr
		push	eax					; BehaviorFlags
		
		; Print the back buffer information.
		mov		ecx, dword [esp+StackSize+pPresentParams]
		mov		eax, dword [ecx+0x28]		; pPresentParams->Flags
		push	eax
		mov		eax, dword [ecx+4]			; pPresentParams->Height
		push	eax
		mov		eax, dword [ecx]			; pPresentParams->Width
		push	eax
		push	Hack_CreateDeviceFormatString
		mov		eax, DbgPrint
		call	eax
		add		esp, 4*4
		
		; Cleanup the stack frame.
		pop		eax
		pop		ecx
		add		esp, StackStart
		
		; Replace the instructions we overwrote.
		push	esi
		push	edi
		mov		edi, ecx
		mov		ecx, dword [0040864Ch]
		
		push	003F524Ah
		ret
		
		%undef pPresentParams
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_IDirect3DDevice8_Swap -> Prevent using the front buffer for rendering
	;---------------------------------------------------------
_Hook_IDirect3DDevice8_Swap:

		; Check if the rasterizer globals have been initialized. Swap will be called once during startup
		; to clear any persistent data in the back buffer memory region and we ignore this first call.
		cmp		dword [Hack_RasterizerTargetsInitialized], 0
		jz		.trampoline

			; Check if debug mode is enabled and draw perf counters if so.
			cmp		dword [Cfg_DebugMode], 0
			jz		.trampoline
			
				; Draw FPS counter.
				call	_Dbg_DrawPerfCounters

.trampoline:

		; Call the trampoline and let the back buffers rotate.
		push	Hook_IDirect3DDevice8_Swap_reentry		; fake return address
		push	ebx
		push	esi
		mov		esi, dword [D3D_g_pDevice]
		push	IDirect3DDevice8_Swap+8
		ret
		
_Hook_IDirect3DDevice8_Swap_reentry:

		; Check if the rasterizer globals have been initialized. Swap will be called once during startup
		; to clear any persistent data in the back buffer memory region and we ignore this first call.
		cmp		dword [Hack_RasterizerTargetsInitialized], 0
		jz		_Hook_IDirect3DDevice8_Swap_exit
		
			; Check if triple buffering is enabled and if so handle triple buffering specific state.
			cmp		byte [Hack_TripleBufferingEnabled], 1
			jnz		_Hook_IDirect3DDevice8_Swap_exit
		
				; Release references to the back buffer from the previous frame.
				push	esi
				push	dword [global_d3d_surface_render_primary_0]
				mov		esi, D3DResource_Release
				call	esi
				
				push	dword [global_d3d_surface_render_primary_1]
				call	esi
				
				; Normally the game is only double buffered and it can utilize the front buffer as a temporary render target
				; for depth of field techniques. However, when triple buffering is enabled the front buffer can be swapped out
				; any time a vblank interval occurs and the next frame is already queued. This causes a flickering effect
				; on anything that uses depth of field passes with the front buffer as one of the targets. To prevent this
				; we update global_d3d_surface_render_primary_0/1 on every swap to point to the active back buffer. This lets
				; the game reference the correct back buffer in the swap chain and "cancels" out any depth of field passes in
				; exchange for eliminating the flickering effect.
				
				; Update addresses for back and front buffer globals. Note that IDirect3DDevice8_GetBackBuffer will increment
				; the ref count on the back buffer surfaces we retrieve which is why we release the reference to the previously
				; acquired back buffer surfaces above.
				push	0
				mov		esi, IDirect3DDevice8_GetBackBuffer
				call	esi
				mov		dword [global_d3d_surface_render_primary_0], eax
				
				push	0
				call	esi
				mov		dword [global_d3d_surface_render_primary_1], eax
				
				; Update the buffer addresses for all render targets based on the front and back buffer.
				mov		esi, dword [global_d3d_surface_render_primary_0]
				mov		esi, dword [esi+4]						; pBackBufferSurface->Data
				
				mov		eax, dword [global_d3d_texture_render_primary_0]
				mov		dword [eax+4], esi						; global_d3d_texture_render_primary_0->Data = pBackBufferSurface->Data
				
				mov		eax, dword [global_d3d_texture_render_primary_1]
				mov		dword [eax+4], esi						; global_d3d_texture_render_primary_1->Data = global_d3d_surface_render_primary_1->Data
				pop		esi
		
_Hook_IDirect3DDevice8_Swap_exit:

		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_physical_memory_allocate -> Adjust game data allocation size
	;---------------------------------------------------------
_Hook_physical_memory_allocate:

		; Decrease the game data allocation size to account for removing texture cache, geometry cache, and rasterizer buffers.
		mov     eax, dword [Hack_RuntimeDataRegionSize]		; 3145000h - 1900000h		; 2000000h
		push    ecx
		push    dword [Hack_RuntimeDataRegionEndAddress]	; 30E4000h - 1900000h		; Reduce the runtime data allocation by 25MB
		
		; Return to original function.
		push	0012B428h
		ret
		
	;---------------------------------------------------------
	; Hook__rasterizer_init_screen_bounds -> Set back buffer resolution based on video mode
	;---------------------------------------------------------
_Hook__rasterizer_init_screen_bounds:

		%define StackSize		14h
		%define StackStart		10h
		%define ScreenWidth		-10h
		%define ScreenHeight	-0Ch
		%define WidthAdjust		-8h
		%define HeightAdjust	-4h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	ecx
		
		mov		dword [esp+StackSize+WidthAdjust], edx
		mov		dword [esp+StackSize+HeightAdjust], ecx
		
		; Set the resolution to 640x480 by default.
		mov		dword [esp+StackSize+ScreenWidth], 640
		mov		dword [esp+StackSize+ScreenHeight], 480
		
		; If widescreen is not enabled on the console force 640x480 or d3d init will fail.
		cmp		byte [_g_widescreen_enabled], 0
		jz		.set_screen_bounds
		
		; Get the video flags and set the screen size based on video mode.
		mov		eax, XGetVideoFlags
		call	eax
		
		; Check if 1080i is disabled and mask out the flag value if so.
		cmp		byte [Cfg_Enable1080iSupport], 0
		jnz		.check_720p_disable
		
			; Mask out 1080i video flag.
			and		eax, ~4				; flags &= ~XC_VIDEO_FLAGS_HDTV_1080i
			
.check_720p_disable:
			
		; Check if 720p is disabled and mask out the flag if so.
		cmp		byte [Cfg_Enable720pSupport], 0
		jnz		.check_video_mode
		
			; Mask out 720p video flag.
			and		eax, ~2				; flags &= ~XC_VIDEO_FLAGS_HDTV_720p
		
.check_video_mode:
		
		; Check video flags and set the screen size based on video mode
		test	eax, 4					; if ((videoFlags & XC_VIDEO_FLAGS_HDTV_1080i) != 0)
		jnz		.video_mode_1080i
		test	eax, 2					; if ((videoFlags & XC_VIDEO_FLAGS_HDTV_720p) != 0)
		jnz		.video_mode_720p
		test	eax, 8					; if ((videoFlags & XC_VIDEO_FLAGS_HDTV_480p) == 0)
		jz		.video_mode_end
		
			; Resolution is 480p
			mov		dword [esp+StackSize+ScreenWidth], 720
			jmp		.video_mode_end
		
.video_mode_1080i:

			; Resolution is 1080i
			mov		dword [esp+StackSize+ScreenWidth], 1920
			mov		dword [esp+StackSize+ScreenHeight], 1080
			jmp		.video_mode_end
		
.video_mode_720p:

			; Resolution is 720p
			mov		dword [esp+StackSize+ScreenWidth], 1280
			mov		dword [esp+StackSize+ScreenHeight], 720

.video_mode_end:

		; If the console doesn't have a RAM upgrade force a max resolution of 480p. There's
		; no point in trying to go any higher because it won't work anyway.
		cmp		byte [Hack_HasRAMUpgrade], 0
		jnz		.set_screen_bounds
		
			; Check if the resolution was set higher than 480p and if so change it.
			cmp		dword [esp+StackSize+ScreenWidth], 720
			jle		.set_screen_bounds
			
				; Force a max resolution of 480p.
				mov		dword [esp+StackSize+ScreenWidth], 720
				mov		dword [esp+StackSize+ScreenHeight], 480

.set_screen_bounds:

		; Set upper left bounds:
		xor		eax, eax
		mov		word [rasterizer_globals_screen_bounds_x0], ax
		mov		word [rasterizer_globals_screen_bounds_y0], ax
		
		; Set lower right bounds:
		mov		eax, dword [esp+StackSize+ScreenWidth]
		mov		word [rasterizer_globals_screen_bounds_x1], ax
		mov		eax, dword [esp+StackSize+ScreenHeight]
		mov		word [rasterizer_globals_screen_bounds_y1], ax
		
		; Set frame bounds:
		mov		eax, dword [esp+StackSize+ScreenWidth]
		mov		ecx, dword [esp+StackSize+WidthAdjust]
		sub		eax, ecx
		mov		word [_rasterizer_globals_frame_bounds_x0], cx
		mov		word [_rasterizer_globals_frame_bounds_x1], ax
		
		mov		eax, dword [esp+StackSize+ScreenHeight]
		mov		ecx, dword [esp+StackSize+HeightAdjust]
		sub		eax, ecx
		mov		word [_rasterizer_globals_frame_bounds_y0], cx
		mov		word [_rasterizer_globals_frame_bounds_y1], ax
		
		; Cleanup stack frame.
		pop		ecx
		add		esp, StackStart
		ret
		
		%undef HeightAdjust
		%undef WidthAdjust
		%undef ScreenHeight
		%undef ScreenWidth
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_rasterizer_preinitialize -> Update present flags based on video mode
	;---------------------------------------------------------
_Hook_rasterizer_preinitialize:

		; Check if 1080i is supported.
		cmp		byte [Cfg_Enable1080iSupport], 1
		jnz		_raster_preinit_exit

			; Get the video flags and check if 1080i is enabled.
			mov		eax, XGetVideoFlags
			call	eax
			test	eax, 4			; if ((XGetVideoFlags() & XC_VIDEO_FLAGS_HDTV_1080i) == 0)
			jz		_raster_preinit_exit
			
				; Enable interlaced mode.
				and		dword [esp+34h], ~40h			; PresentParams.Flags &= ~D3DPRESENTFLAG_PROGRESSIVE
				or		dword [esp+34h], 20h			; PresentParams.Flags |= D3DPRESENTFLAG_INTERLACED
		
_raster_preinit_exit:

		; Replace the instructions we overwrote.
		lea		ecx, [esp+0Ch]
		push	ecx
		mov		ecx, global_d3d_device
		
		push	00012521h
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_rasterizer_device_initialize -> Update present flags based on video mode
	;---------------------------------------------------------
_Hook_rasterizer_device_initialize:	

		; Set the refresh rate.
		movsx	edx, word [_g_refresh_rate_hz]
		mov		dword [esp+38h], edx				; PresentParams.FullScreen_RefreshRateInHz = _g_refresh_rate_hz
		
		; Check if widescreen mode is enabled.
		mov		al, byte [_g_widescreen_enabled]
		cmp		al, 0
		jz		_check_progressive_scan
		
			; Enable widescreen mode.
			or		dword [esp+34h], 10h			; PresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN
		
_check_progressive_scan:

		; Check if progressive scan is enabled (480p/720p resolutions).
		mov		al, byte [_g_progressive_scan_enabled]
		cmp		al, 0
		jz		_check_interlaced
		
			; Enable progressive scan mode.
			or		dword [esp+34h], 40h			; PresentParams.Flags |= D3DPRESENTFLAG_PROGRESSIVE
			
_check_interlaced:

		; Check the resolution width to see if 1080i is enabled.
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]
		cmp		eax, 1920
		jnz		_device_init_exit
		
			; Enable interlaced mode.
			and		dword [esp+34h], ~40h			; PresentParams.Flags &= ~D3DPRESENTFLAG_PROGRESSIVE
			or		dword [esp+34h], 20h			; PresentParams.Flags |= D3DPRESENTFLAG_INTERLACED
			
_device_init_exit:

		; If triple buffering is enabled change the present interval and buffer count.
		cmp		byte [Hack_TripleBufferingEnabled], 1
		jnz		.exit

			; Set swap effect and present interval.
			mov		dword [esp+18h], 2					; BackBufferCount = 2
			mov		dword [esp+20h], 1					; SwapEffect = D3DSWAPEFFECT_DISCARD
			mov		dword [esp+3Ch], 1					; PresentationInterval = D3DPRESENT_INTERVAL_ONE
		
.exit:

		; Replace instructions we overwrote.
		mov		eax, dword [global_d3d_device]
		test	eax, eax
		jnz		_device_init_reset
		
		push	000129BEh
		ret
		
_device_init_reset:

		push	000129D4h
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_rasterizer_primary_targets_initialize -> Use screen resolution for texture sizes
	;---------------------------------------------------------
_Hook_rasterizer_primary_targets_initialize:

		%define StackSize		24h
		%define StackStart		1Ch
		%define TileInfo		-1Ch
		%define ReturnValue		-4h

		; Setup the stack frame.
		sub		esp, StackStart
		push	ecx
		push	esi
		
		; Trampoline to the real function and create the render textures.
		push	Hook_rasterizer_primary_targets_initialize_reentry		; Fake return address the real function will return to
		push	ecx
		push	ebx
		push	ebp
		push	esi
		mov		esi, dword [D3D_g_pDevice]		; g_pDevice
		
		; Check if triple buffering is enabled and apply fixes if needed.
		cmp		byte [Hack_TripleBufferingEnabled], 1
		jnz		.triple_buffering_disabled

			; We can't reference the front buffer when triple buffering is enabled. Set global_d3d_surface_render_primary_0/1
			; to point to the active back buffer. When D3DDevice_Swap is called Hook_IDirect3DDevice8_Swap will handle swapping
			; all surface/texture buffer addresses to point to the active buffer. Internally the game will still flip its accesses
			; to global_d3d_surface_render_primary_0/1 every frame, but they point to the same underlying buffer so it doesn't matter.
			
			push	0
			mov		eax, IDirect3DDevice8_GetBackBuffer
			call	eax
			mov		dword [global_d3d_surface_render_primary_0], eax
			
			push	0
			mov		eax, IDirect3DDevice8_GetBackBuffer
			call	eax
			mov		dword [global_d3d_surface_render_primary_1], eax
			
			; Skip over part of the original function now that we already setup global_d3d_surface_render_primary_0/1.
			xor     ebx, ebx
			mov     byte [esp+0Fh], 1
			push	00014671h
			ret

.triple_buffering_disabled:

		; Let the original function reference the back and front buffers.
		push	0001460Ah
		ret
		
_Hook_rasterizer_primary_targets_initialize_reentry:

		; Save the return value.
		mov		dword [esp+StackSize+ReturnValue], eax
		
		; Update global_d3d_texture_render_primary:
		push 	dword [global_d3d_texture_render_primary_0]
		call	_Hack_UpdateD3dPixelContainerForScreenResolution
		
		push	dword [global_d3d_texture_render_primary_1]
		call	_Hack_UpdateD3dPixelContainerForScreenResolution
		
		; Update global texaccum:
		push	dword [global_d3d_texture_z_as_target_0]
		call	_Hack_UpdateD3dPixelContainerForScreenResolution
		
		; Update unknown texture:
		push	dword [global_d3d_texture_z_as_target_1]
		call	_Hack_UpdateD3dPixelContainerForScreenResolution
		
		; Check if we should disable depth buffer compression.
		cmp		byte [Hack_DisableZCompress], 0
		jz		_Hook_rasterizer_primary_targets_initialize_exit
		
			; Get the tile info for the depth buffer.
			lea		eax, [esp+StackSize+TileInfo]
			push	eax
			push	1
			mov		eax, IDirect3DDevice8_GetTile
			call	eax
			
			; Turn off compression for the depth buffer.
			mov		eax, dword [esp+StackSize+TileInfo+D3DTILE.Flags]
			and		eax, ~80000000h										; TileInfo.Flags &= ~D3DTILE_FLAGS_ZCOMPRESS
			mov		dword [esp+StackSize+TileInfo+D3DTILE.Flags], eax
			
			; Clear the old depth buffer tile (required in order for new tile info to take effect).
			xor		ecx, ecx							; pTile = NULL
			mov		eax, 1								; Index = 1
			mov		esi, IDirect3DDevice8_SetTile
			call	esi
			
			; Save the new tile info.
			lea		ecx, [esp+StackSize+TileInfo]		; pTile = &TileInfo
			mov		eax, 1								; Index = 1
			mov		esi, IDirect3DDevice8_SetTile
			call	esi
		
_Hook_rasterizer_primary_targets_initialize_exit:

		; Set the flag for render target flipping.
		mov		dword [Hack_RasterizerTargetsInitialized], 1

		; Cleanup stack frame.
		mov		eax, dword [esp+StackSize+ReturnValue]				; Original return value
		pop		esi
		pop		ecx
		add		esp, StackStart
		ret
		
		%undef ReturnValue
		%undef TileInfo
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hack_UpdateD3dPixelContainerForScreenResolution -> Update a D3DPIXELCONTAINER structure to match the screen resolution
	;---------------------------------------------------------
_Hack_UpdateD3dPixelContainerForScreenResolution:

		%define StackSize		8h
		%define StackStart		0h
		%define pTexture		4h
		
		; Setup the stack frame.
		sub		esp, StackStart
		push	ecx
		push	edx
		
		; Calculate the pitch for the texture based on format and screen width.
		mov		eax, dword [esp+StackSize+pTexture]
		mov		eax, dword [eax+0Ch]
		shr		eax, 8
		and		eax, 0FFh											; (pTexture->format >> 8) & 0xFF
		movsx	ecx, word [rasterizer_globals_screen_bounds_x1]
		push	ecx													; screen_width
		mov		edx, D3D_CalcTilePitch
		call	edx
		mov		edx, eax
		
		; Setup the new texture resolution bits.
		movsx	ecx, word [rasterizer_globals_screen_bounds_x1]
		sub		ecx, 1
		and		ecx, 0FFFh
		movsx	eax, word [rasterizer_globals_screen_bounds_y1]
		sub		eax, 1
		and		eax, 0FFFh
		shl		eax, 12
		or		ecx, eax
		
		; Setup the new texture pitch bits.
		shr		edx, 6
		sub		edx, 1				; (pitch / 64) - 1
		shl		edx, 24
		and		edx, 0FF000000h
		or		ecx, edx
		
		; Update the texture.
		mov		edx, dword [esp+StackSize+pTexture]
		mov		dword [edx+10h], ecx						; pTexture->size = ...
		
		; Cleanup stack frame.
		pop		edx
		pop		ecx
		add		esp, StackStart
		ret 4
		
		%undef pTexture
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__rasterizer_create_render_target -> Change render target texture size based on screen resolution
	;---------------------------------------------------------
_Hook__rasterizer_create_render_target:

		%define StackSize		4h
		%define StackStart		0h
		%define TextureWidth	8h
		%define TextureHeight	0Ch
		
		; TODO: Change this to hook rasterizer_targets_initialize instead and combine with the
		;	allocation size change patch as well...
		
		; Setup the stack frame.
		sub		esp, StackStart
		push	eax					; rasterizer_target
		
		; Check the render target texture id.
		cmp		eax, 1
		jz		_fix_render_target_1
		
		; Don't make any updates
		jmp		_create_render_target_exit
		
_fix_render_target_1:

		;INT3

		; Update width and height using screen resolution.
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]
		mov		dword [esp+StackSize+TextureWidth], eax
		movsx	eax, word [rasterizer_globals_screen_bounds_y1]
		mov		dword [esp+StackSize+TextureHeight], eax
		jmp		_create_render_target_exit
		
_create_render_target_exit:

		; Cleanup the stack frame.
		pop		eax
		add		esp, StackStart
		
		; Replace the instructions we overwrote.
		push	ecx
		push	ebp
		mov		ebp, [esp+24h]
		push	_rasterizer_create_render_target+6
		ret
		
		%undef TextureHeight
		%undef TextureWidth
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_rasterizer_targets_initialize -> Change render target texture size based on screen resolution
	;---------------------------------------------------------
_Hook_rasterizer_targets_initialize:

		; Replace instructions we overwrote and use screen resolution for render target texture size.
		push	ebx
		push	0
		push	1
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]			; screen_width
		push	eax
		push	1														; rasterizer_target
		movsx	ebx, word [rasterizer_globals_screen_bounds_y1]			; screen_height
		push	0001D781h		; rasterizer_targets_initialize+11h
		ret

		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__rasterizer_alloc_and_create_render_target -> Change render target size and allocation method for render target 1 (texaccum target)
	;---------------------------------------------------------
_Hook__rasterizer_alloc_and_create_render_target:

		%define TargetIndex			4h
		%define Width				8h
		%define z_surface			10h
		
		; ebx = height
		
		; Check the render target index for the texaccum targer.
		cmp		dword [esp+TargetIndex], 1
		jnz		.exit
		
			; ecx can be trashed by this function per the original xbe
		
			; Calculate the size of the render target buffer. Note this doesn't account for tile pitch but
			; we aren't tiling the buffer anyway so it doesn't matter.
			movsx	ecx, word [rasterizer_globals_screen_bounds_x1]			; screen_width
			lea		ecx, [ecx+63]
			and		ecx, ~63												; (screen_width + 63) & ~63
			movsx	eax, word [rasterizer_globals_screen_bounds_y1]			; screen_height
			mul		ecx
			shl		eax, 2													; size = ((screen_width + 63) & ~63) * screen_height * 4 (32bpp)
			
			; Allocate a new buffer in upper 64MB.
			push	404h													; PAGE_WRITECOMBINE | PAGE_READWRITE
			push	eax
			push	PHYS_MEM_REGION_RASTERIZER_TEXACCUM_TARGET
			call	_Hack_PhysicalMemoryAlloc
			
			; Create the rasterizer target.
			or		eax, 80000000h
			push	eax														; buffer address
			push	1
			push	1														; linear = true
			push	dword [esp+z_surface+0Ch]								; z_surface
			movsx	eax, word [rasterizer_globals_screen_bounds_y1]			; screen_height
			push	eax
			movsx	eax, word [rasterizer_globals_screen_bounds_x1]			; screen_width
			push	eax
			push	1														; type = static
			mov		eax, dword [esp+TargetIndex+1Ch]						; eax = render target index
			mov		ecx, _rasterizer_create_render_target
			call	ecx
			
			ret 10h
		
.exit:

		; Replace the instructions we overwrote, run the function as normal.
		mov		edx, dword [physical_memory_globals_current_stage]
		push	_rasterizer_alloc_and_create_render_target+6
		ret
		
		align 4, db 0
		
		%undef z_surface
		%undef Width
		%undef TargetIndex
		
	;---------------------------------------------------------
	; Hook_rasterizer_get_render_target_resolution -> Change resolution for render targets (0, 3, 24) with hard coded values
	;---------------------------------------------------------
_Hook_rasterizer_get_render_target_resolution:

		; Render target 0: front buffer
		; Render target 3: back buffer
		; Render target 24: ?

		; Use the video resolution for the render target resolution.
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]
		mov		dword [edx], eax									; *resolution_x = screen_width
		movsx	eax, word [rasterizer_globals_screen_bounds_y1]
		mov		dword [ecx], eax									; *resolution_y = screen_height
		
		; Replace the instructions we overwrote.
		pop		edi
		mov		al, 1
		pop		esi
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_create_render_target_helper -> Use screen resolution for creating render target 18 (memory is stolen from the standard texture cache)
	;---------------------------------------------------------
_Hook_create_render_target_helper:

		; Replace instructions we overwrote.
		mov		eax, dword [00485898h]
		push	ebp
		
		; Use screen resolution for render target size.
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]
		mov		dword [esp+4+4], eax
		movsx	eax, word [rasterizer_globals_screen_bounds_y1]
		mov		dword [esp+4+8], eax
		
		push	0001DC46h
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook_should_render_screen_effect -> Use proper screen resolution for screen effect check
	;---------------------------------------------------------
_Hook_should_render_screen_effect:

		; Use video resolution for window size check.
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]
		cmp		ax, word [0048564Ch + 2]
		jnz		_Hook_should_render_screen_effect_fail
		
		movsx	eax, word [rasterizer_globals_screen_bounds_y1]
		cmp		ax, word [0048564Ch]
		jnz		_Hook_should_render_screen_effect_fail
		
		xor		al, al
		push	0002273Ch
		ret
		
_Hook_should_render_screen_effect_fail:

		xor		al, al
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__fog_build_vertex_element -> Use correct texture coordinates
	;---------------------------------------------------------
_Hook__fog_build_vertex_element:

		%define StackSize			4h
		%define StackStart			0h
		%define RegisterIndex		4h
		%define Bounds				8h
		%define pInputVector		0Ch
		%define pOutputVector		10h
		%define arg_10				14h
		
		; Setup stack frame.
		push	ecx
		
		; Call the original function.
		push	dword [esp+StackSize+arg_10]
		push	dword [esp+StackSize+pOutputVector+4]
		push	dword [esp+StackSize+pInputVector+8]
		push	dword [esp+StackSize+Bounds+0Ch]
		push	dword [esp+StackSize+RegisterIndex+10h]
		mov		eax, 00027520h
		call	eax
		
		; Only modify the output values for vertex element 4.
		cmp		dword [esp+StackSize+RegisterIndex], 4
		jnz		_Hook__fog_build_vertex_element_exit

		; Use correct texture coordinates for fog passes.
		mov		ecx, dword [esp+StackSize+pOutputVector]
		cmp		dword [ecx], 0
		jz		_Hook__fog_build_vertex_element_y
		
		; Map the texcoords for the fog render target texture as (320, 240) -> (0, 1).
		mov		dword [ecx], __?float32?__(320.0)
		
_Hook__fog_build_vertex_element_y:

		cmp		dword [ecx+4], 0
		jz		_Hook__fog_build_vertex_element_exit
		
		mov		dword [ecx+4], __?float32?__(240.0)
		
_Hook__fog_build_vertex_element_exit:

		; Cleanup stack frame.
		pop		ecx
		ret 14h
		
		%undef arg_10
		%undef pOutputVector
		%undef pInputVector
		%undef Bounds
		%undef RegisterIndex
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__draw_split_screen_window_bars -> Use screen resolution for split screen bar placement
	;---------------------------------------------------------
_Hook__draw_split_screen_window_bars:

		%define StackSize		1Ch
		%define StackStart		10h
		%define RectBounds		-10h
		%define HalfWidth		-8h
		%define HalfHeight		-4h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	esi
		push	ecx
		push	edx
		
		; Make sure there's at least one player window.
		cmp		dword [render_globals_player_window_count], 1
		jle		_Hook__draw_split_screen_window_bars_exit
		
		; Calculate the half width/height bounds.
		mov		ecx, 2
		xor		edx, edx
		movsx	eax, word [rasterizer_globals_screen_bounds_x1]
		idiv	ecx
		mov		dword [esp+StackSize+HalfWidth], eax
		movsx	eax, word [rasterizer_globals_screen_bounds_y1]
		idiv	ecx
		mov		dword [esp+StackSize+HalfHeight], eax
		
		; Check the screen split mode.
		lea		ecx, [esp+StackSize+RectBounds]
		cmp		dword [004BA048h], 1
		jnz		_Hook__draw_split_screen_window_bars_mode_1_2
		
			; Mode 0: draw vertical split
			mov		eax, dword [esp+StackSize+HalfHeight]
			mov		word [ecx+0], ax
			dec		word [ecx+0]										; pRect->x0 = HalfHeight - 1
			mov		word [ecx+2], 0										; pRect->y0 = 0
			mov		word [ecx+4], ax
			inc		word [ecx+4]										; pRect->x1 = HalfHeight + 1
			mov		ax, word [rasterizer_globals_screen_bounds_x1]
			mov		word [ecx+6], ax									; pRect->y1 = screen_width
			
			mov		eax, 0FF000000h										; color = black
			mov		esi, _renderer_draw_color_rect
			call	esi
			
			; Check if there's more than 2 players.
			cmp		dword [render_globals_player_window_count], 2
			jle		_Hook__draw_split_screen_window_bars_exit
			
				; Mode 0: draw horizontal split
				lea		ecx, [esp+StackSize+RectBounds]
				mov		eax, dword [esp+StackSize+HalfWidth]
				mov		word [ecx+2], ax
				dec		word [ecx+2]										; pRect->y0 = halfWidth - 1
				mov		word [ecx+6], ax
				inc		word [ecx+6]										; pRect->y1 = halfWidth + 1
				mov		ax, word [rasterizer_globals_screen_bounds_y1]
				mov		word [ecx+4], ax									; pRect->x1 = screen_height
				
				xor		eax, eax
				cmp		dword [render_globals_player_window_count], 3
				setz	al
				mul		dword [esp+StackSize+HalfHeight]
				mov		word [ecx], ax										; pRect->x0 = render_globals_player_window_count == 3 ? halfHeight : 0
				
				mov		eax, 0FF000000h										; color = black
				mov		esi, _renderer_draw_color_rect
				call	esi
				jmp		_Hook__draw_split_screen_window_bars_exit
		
_Hook__draw_split_screen_window_bars_mode_1_2:

		; Mode 1/2: draw vertical split
		mov		eax, dword [esp+StackSize+HalfWidth]
		mov		word [ecx+0], 0										; pRect->x0 = 0
		mov		word [ecx+2], ax
		dec		word [ecx+2]										; pRect->y0 = halfWidth - 1
		mov		word [ecx+6], ax
		inc		word [ecx+6]										; pRect->y1 = halfWidth + 1
		mov		eax, dword [rasterizer_globals_screen_bounds_y1]
		mov		word [ecx+4], ax									; pRect->x1 = screen_height
		
		mov		eax, 0FF000000h										; color = black
		mov		esi, _renderer_draw_color_rect
		call	esi
		
		; Check if there's more than 2 players.
		cmp		dword [render_globals_player_window_count], 2
		jle		_Hook__draw_split_screen_window_bars_exit
		
			; Mode 1/2: draw horizontal split
			lea		ecx, [esp+StackSize+RectBounds]
			mov		eax, dword [esp+StackSize+HalfHeight]
			mov		word [ecx+0], ax
			dec		word [ecx+0]										; pRect->x0 = halfHeight - 1
			mov		word [ecx+4], ax
			inc		word [ecx+4]										; pRect->x1 = halfHeight + 1
			mov		ax, word [rasterizer_globals_screen_bounds_x1]
			mov		word [ecx+6], ax									; pRect=>y1 = screen_width
			
			xor		eax, eax
			cmp		dword [render_globals_player_window_count], 3
			setz	al
			mul		dword [esp+StackSize+HalfWidth]
			mov		word [ecx+2], ax									; pRect->x0 = render_globals_player_window_count == 3 ? halfWidth : 0
			
			mov		eax, 0FF000000h										; color = black
			mov		esi, _renderer_draw_color_rect
			call	esi
		
_Hook__draw_split_screen_window_bars_exit:

		; Cleanup stack frame.
		pop		edx
		pop		ecx
		pop		esi
		add		esp, StackStart
		ret
		
		%undef HalfHeight
		%undef HalfWidth
		%undef RectBounds
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__renderer_setup_player_windows -> Allow customization of split screen favor (horizontal vs vertical split)
	;---------------------------------------------------------
_Hook__renderer_setup_player_windows:

		; Check if widescreen mode is enabled.
		cmp		byte [_g_widescreen_enabled], 0
		jnz		.widescreen_enabled
		
			; Use default setting (horizontal split).
			mov		dword [esp+10h], 1
			jmp		.exit
		
.widescreen_enabled:

		; Use value specified by config file.
		mov		eax, dword [Cfg_SplitScreenFavor]
		CLAMP	eax, 1, 2
		mov		dword [esp+10h], eax

.exit:

		; Return to function.
		push	00223585h
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__should_draw_player_hud -> Disable player HUD
	;---------------------------------------------------------
_Hook__should_draw_player_hud:

		; Disable HUD.
		xor		al, al
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; void Hook__initialize_geometry_cache() -> increase the size of the geometry cache if the console has 128MB of RAM
	;---------------------------------------------------------
_Hook__initialize_geometry_cache:

		; Check if the console has 128MB of RAM.
		cmp		byte [Hack_HasRAMUpgrade], 1
		jnz		.no_ram_upgrade
		
			; Increase the size of the geometry cache.
			mov		eax, EXPANDED_GEOMETRY_CACHE_SIZE
			
			; eax = size (finish size calculation)
			push	ebp
			push	esi
			push	edi
			mov		esi, eax
			sar     esi, 0Ch					; Used later in original function
			mov     edi, esi
			shl     edi, 0Ch					; Used later in original function
			lea     ecx, [edi+0FFFh]
			and     ecx, 0FFFFF000h
			
			push	4							; PAGE_READWRITE
			push	ecx							; size must be in ecx for subsequent XPhysicalProtect call
			push	PHYS_MEM_REGION_GEOMETRY_CACHE
			call	_Hack_PhysicalMemoryAlloc
			
			push	0012DA73h
			ret
		
.no_ram_upgrade:

		; No RAM upgrade present, replace instructions we overwrote.
		cdq
		and		edx, 0FFFh
		push	0012DA2Eh
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__geometry_cache_globals_cleanup -> Free geometry cache memory
	;---------------------------------------------------------
_Hook__geometry_cache_globals_cleanup:

		HACK_FUNCTION geometry_cache_globals_cleanup_reentry

		; Trampoline to the original function.
		push	geometry_cache_globals_cleanup_reentry		; Push fake return address
		mov		eax, dword[004E649Ch]						; xbox_geometry_cache_globals_cache
		test	eax, eax
		push	0012DAD7h
		ret
		
_geometry_cache_globals_cleanup_reentry:

		; Free the allocation.
		push	PHYS_MEM_REGION_GEOMETRY_CACHE
		call	_Hack_PhysicalMemoryFree
		
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; void Hook__initialize_standard_texture_cache() -> increase the size of the standard texture cache if the console has 128MB of RAM
	;---------------------------------------------------------
_Hook__initialize_standard_texture_cache:

		; Replace instructions we overwrote.
		push	ebx
		push	ebp
		push	esi
		push	edi
		mov     edi, dword [physical_memory_globals_current_stage]

		; Check if the console has 128MB of RAM.
		cmp		byte [Hack_HasRAMUpgrade], 1
		jnz		.no_ram_upgrade

			; Get a pointer to the region info data.
			mov		eax, PHYS_MEM_REGION_TEXTURE_CACHE
			imul	eax, 0Ch
			add		eax, Hack_PhysicalMemoryRegionInfoTable		; pRegionInfo = &Hack_PhysicalMemoryRegionInfoTable[RegionIndex]
			mov		esi, dword [eax+4]							; pRegionInfo->size
			mov		ecx, esi									; Used later in original function
			sar     esi, 0Ch									; Used later in original function
			
			push	404h										; PAGE_WRITECOMBINE | PAGE_READWRITE
			push	ecx											; size must be in ecx for subsequent XPhysicalProtect call
			push	PHYS_MEM_REGION_TEXTURE_CACHE
			call	_Hack_PhysicalMemoryAlloc
			
			push	0012C23Eh
			ret
		
.no_ram_upgrade:

		; No RAM upgrade present, run original function.
		push	_initialize_standard_texture_cache+0Ah
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hook__texture_cache_globals_cleanup -> Free texture cache memory
	;---------------------------------------------------------
_Hook__texture_cache_globals_cleanup:

		HACK_FUNCTION Hook__texture_cache_globals_cleanup_reentry

		; Trampoline to the original function.
		push	Hook__texture_cache_globals_cleanup_reentry		; Push fake return address
		push	ebx
		mov		dword [004E6479h], 1
		push	0012C298h
		ret
		
_Hook__texture_cache_globals_cleanup_reentry:

		; Free the allocation.
		push	PHYS_MEM_REGION_TEXTURE_CACHE
		call	_Hack_PhysicalMemoryFree
		
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; Hack_PhysicalMemoryAlloc(int regionIndex, int size, int protect) -> Allocate physical contiguous memory in the extra RAM region
	;---------------------------------------------------------
_Hack_PhysicalMemoryAlloc:

		%define StackSize		8h
		%define StackStart		0h
		%define RegionIndex		4h
		%define AllocSize		8h
		%define Protect			0Ch
		
		; Setup stack frame.
		sub		esp, StackStart
		push	esi
		push	edx
		
		; Get a pointer to the region info data.
		mov		eax, 0Ch
		mul		dword [esp+StackSize+RegionIndex]
		mov		esi, Hack_PhysicalMemoryRegionInfoTable
		add		esi, eax									; pRegionInfo = &Hack_PhysicalMemoryRegionInfoTable[RegionIndex]
		
		; If a size was specified use it for the allocation.
		mov		eax, dword [esp+StackSize+AllocSize]
		cmp		eax, 0
		jz		_Hack_PhysicalMemoryAlloc_alloc
		
			mov		dword [esi+4], eax						; pRegionInfo->size = AllocSize
		
_Hack_PhysicalMemoryAlloc_alloc:

		; Allocate physical contiguous memory. The memory manager will search for an available memory region
		; starting at the end of RAM and working towards the beginning. If no contiguous region is found it
		; will relocate virtual allocations to satisfy the request.
		mov		eax, dword [esi+4]
		add		eax, 0FFFh
		and		eax, ~0FFFh								; round size to nearest page size
		push	dword [esp+StackSize+Protect]			; page protection
		push	4096									; alignment = PAGE_SIZE
		push	0FFFFFFFFh								; HighestAcceptableAddress = highest mem address
		push	0										; LowestAcceptableAddress = lowest mem address
		push	eax										; size = pRegionInfo->size
		call	dword [MmAllocateContiguousMemoryEx]
		mov		dword [esi+8], eax						; pRegionInfo->address = MmAllocateContiguousMemoryEx(...)
		cmp		eax, 0
		jnz		_Hack_PhysicalMemoryAlloc_dbg_print
		
		; Allocation failed :(
		INT3
		
_Hack_PhysicalMemoryAlloc_dbg_print:

		; Print the allocation info.
		push	eax										; address
		push	dword [esi+4]							; pRegionInfo->size
		push	dword [esi+0]							; pRegionInfo->name
		push	Hack_PhysicalMemoryMallocInfoString
		mov		eax, DbgPrint
		call	eax
		add		esp, 4*4
		
		; Cleanup stack frame.
		mov		eax, dword [esi+8]			; return pRegionInfo->address
		pop		edx
		pop		esi
		add		esp, StackStart
		ret 0Ch
		
		align 4, db 0
		
		%undef Protect
		%undef AllocSize
		%undef RegionIndex
		%undef StackStart
		%undef StackSize
		
	;---------------------------------------------------------
	; Hack_PhysicalMemoryFree -> Free physical memory allocation
	;---------------------------------------------------------
_Hack_PhysicalMemoryFree:

		%define StackSize		4h
		%define StackStart		0h
		%define RegionIndex		4h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	esi
		
		; Get a pointer to the region info data.
		mov		eax, 0Ch
		mul		dword [esp+StackSize+RegionIndex]
		mov		esi, Hack_PhysicalMemoryRegionInfoTable
		add		esi, eax									; pRegionInfo = &Hack_PhysicalMemoryRegionInfoTable[RegionIndex]
		
		; Check if the allocation address is valid.
		cmp		dword [esi+8], 0
		jz		_Hack_PhysicalMemoryFree_exit

		; Free the allocation.
		push	dword [esi+8]
		call	dword [MmFreeContiguousMemory]
		mov		dword [esi+8], 0
		
_Hack_PhysicalMemoryFree_exit:

		; Cleanup stack frame.
		pop		esi
		add		esp, StackStart
		ret 4
		
		align 4, db 0
		
		%undef RegionIndex
		%undef StackStart
		%undef StackSize
		
	;---------------------------------------------------------
	; void Hack_PatchMaxPFN()
	;---------------------------------------------------------
_Hack_PatchMaxPFN:

		; Setup the stack frame.
		push	esi
		push	edi
		
		; Get the address range for MmAllocateContiguousMemoryEx.
		mov		esi, dword [MmAllocateContiguousMemoryEx]
		lea		edi, [esi+80h]
		
_Hack_PatchMaxPFN_loop:

		; Check for the following instruction that holds the max PFN to allocate.
		;	mov		xxx, 00003FDFh
		cmp		dword [esi], 00003FDFh
		jz		_Hack_PatchMaxPFN_patch
		
		; Next iteration.
		add		esi, 1
		cmp		esi, edi
		jl		_Hack_PatchMaxPFN_loop
		
		; Max PFN value was not found.
		INT3
		
_Hack_PatchMaxPFN_patch:

		; Disable write protect.
		pushf
		cli							; Disable interrupts
		mov		eax, cr0			; Get the control register value
		push	eax					; Save it for later
		and		eax, 0FFFEFFFFh		; Disable write-protect
		mov		cr0, eax

		; Update the max PFN to use the arcade limit (128MB).
		mov		dword [esi], 00007FCFh
		
		; Re-enable write-protect.
		pop		eax
		mov		cr0, eax			; Re-enable write-protect
		popf
		
_Hack_PatchMaxPFN_exit:

		; Cleanup stack frame.
		pop		edi
		pop		esi
		ret
		
		align 4, db 0
		
	;---------------------------------------------------------
	; void Hack_ColdRebootConsole() -> Cold reboot the console when any other executable is launched
	;---------------------------------------------------------
_Hack_ColdRebootConsole:

		; If a custom fan speed was used change it back to normal.
		cmp		dword [Cfg_OverrideFanSpeed], 0
		jz		.check_irql
		
			; Set the fan speed back to stock.
			push	10								; fan speed
			push	0
			push	6								; SMC_COMMAND_REQUEST_FAN_SPEED
			push	20h								; SMC_SLAVE_ADDRESS
			call	dword [HalWriteSMBusValue]
			
			; Give the SMC a chance to process the message, if not it can panic.
			push	100*1000						; 100ms
			call	dword [KeStallExecutionProcessor]
		
			; Set SMC fan mode to use stock speed (this doesn't seem to work if the override speed is still set).
			push	0								; SMC_FAN_OVERRIDE_DEFAULT
			push	0
			push	5								; SMC_COMMAND_FAN_OVERRIDE
			push	20h								; SMC_SLAVE_ADDRESS
			call	dword [HalWriteSMBusValue]
			
			; Give the SMC a chance to process the message, if not it can panic.
			push	100*1000						; 100ms
			call	dword [KeStallExecutionProcessor]
			
.check_irql:

		; Check IRQL level to see if we can use HalWriteSMBusValue and if so have the SMC do a full reset for us.
		call	dword [KeGetCurrentIrql]
		cmp		eax, 2									; if (KeGetCurrentIrql < DISPATCH_LEVEL)
		jnb		.do_pci_reset
		
			; Have the SMC do a full reset.
			push	1									; SMC_RESET_ASSERT_RESET
			push	0									; FALSE
			push	2									; SMC_COMMAND_RESET
			push	20h									; SMC_SLAVE_ADDRESS
			call	dword [HalWriteSMBusValue]
		
.do_pci_reset:

		; Perform full PCI reset.
		mov		dx, 0CF9h				; RESET_CONTROL_REGISTER
		mov		al, 0Eh					; RESET_CONTROL_FULL_RESET | RESET_CONTROL_RESET_CPU | RESET_CONTROL_SYSTEM_RESET
		out		dx, al
		
.halt:
		
		; Halt the CPU and wait for the reboot, thanks for playing...
		cli
		hlt
		jmp		.halt
		
		align 4, db 0
		
	;---------------------------------------------------------
	; void _crc32_calculate_stdcall(int* checksum, unsigned char* buffer, int length) -> stub for fastcall version of _crc32_calculate_opt
	;---------------------------------------------------------
_crc32_calculate_stdcall:

		%define StackSize		0Ch
		%define StackStart		0h
		%define Checksum		4h
		%define Buffer			8h
		%define Length			0Ch
		
		; Setup stack frame.
		sub		esp, StackStart
		push	edi
		push	ecx
		push	edx		; trashed by _crc32_calculate
		
		; Call fastcall version of _crc32_calculate.
		mov		edi, dword [esp+StackSize+Length]
		mov		eax, dword [esp+StackSize+Buffer]
		push	dword [esp+StackSize+Checksum]
		mov		ecx, _crc32_calculate_opt
		call	ecx
		
		; Cleanup stack frame.
		pop		edx
		pop		ecx
		pop		edi
		add		esp, StackStart
		ret
		
		align 4, db 0
		
		%undef Length
		%undef Buffer
		%undef Checksum
		%undef StackStart
		%undef StackSize
		
		
	;---------------------------------------------------------
	; Include files that need to be in the .hacks segment:
	;---------------------------------------------------------
	%include "Utilities.asm"
	%include "Config.asm"
	%include "DirectX.asm"
	%include "Debug.asm"

	;---------------------------------------------------------
	; A poor man's data segment...
	;---------------------------------------------------------
		
_Util_KernelImports:
		_HalReadWritePCISpace				dd 46
		_HalReturnToFirmware				dd 49
		_HalWriteSMBusValue					dd 50
		_KeGetCurrentIrql					dd 103
		_KeStallExecutionProcessor			dd 151
		_MmAllocateContiguousMemoryEx		dd 166
		_MmFreeContiguousMemory				dd 171
											dd 0
											
_Hack_StartupMessage:
		db `Halo 2 HD initializing...\n`,0
		align 4, db 0
		
_Hack_CreateDeviceFormatString:
		db `IDirect3D8_CreateDevice Width=%d Height=%d Flags=0x%08x\n`,0
		align 4, db 0
		
_Hack_PhysicalMemoryMallocInfoString:
		db `physical_memory_malloc %s %ld at 0x%08x\n`,0
		align 4, db 0
		
		
	PHYS_MEM_REGION_STR PHYS_MEM_REGION_GEOMETRY_CACHE_STR, `geometry cache`
	PHYS_MEM_REGION_STR PHYS_MEM_REGION_TEXTURE_CACHE_STR, `texture cache`
	PHYS_MEM_REGION_STR PHYS_MEM_REGION_RASTERIZER_TEXACCUM_TARGET_STR, `rasterizer texaccum target`
		
_Hack_PhysicalMemoryRegionInfoTable:
		PHYS_MEM_REGION PHYS_MEM_REGION_GEOMETRY_CACHE_STR, 0				; Set in hook
		PHYS_MEM_REGION PHYS_MEM_REGION_TEXTURE_CACHE_STR, EXPANDED_TEXTURE_CACHE_SIZE
		PHYS_MEM_REGION PHYS_MEM_REGION_RASTERIZER_TEXACCUM_TARGET_STR, 0
		
		
_Hack_RasterizerTargetsInitialized:
		dd 0
		
_Hack_HasRAMUpgrade:				db 0
_Hack_TripleBufferingEnabled:		db 0
_Hack_DisableZCompress:				db 1

		align 4, db 0
		
_Hack_RuntimeDataRegionSize:		dd 3145000h		; ~49.2MB
_Hack_RuntimeDataRegionEndAddress:	dd 30E4000h

_hacks_code_end:

; Safety cehck to make sure we don't overflow the .hacks segment
%if (_hacks_code_end - _hacks_code_start) > HacksSegmentSize
	%error ".hacks segment overflow, segment needs to be expanded!"
%endif

; ////////////////////////////////////////////////////////
; //////////////////// End of file ///////////////////////
; ////////////////////////////////////////////////////////
dd -1