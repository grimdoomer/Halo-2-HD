

	;---------------------------------------------------------
	; void Dbg_DrawPerfCounters()
	;---------------------------------------------------------
_Dbg_DrawPerfCounters:

		%define StackSize			64h
		%define StackStart			5Ch
		%define TempFloat			-5Ch
		%define Bounds				-58h
		%define vColor				-50h
		%define StringBuffer		-40h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	ecx
		push	esi
		
		; Setup string bounds.
		lea		ecx, [esp+StackSize+Bounds]
		mov		word [ecx+RECT.upper], 100
		mov		word [ecx+RECT.lower], 120
		mov		word [ecx+RECT.left], 30
		mov		word [ecx+RECT.right], 200
		
		; Setup text color.
		mov		dword [esp+StackSize+vColor], __?float32?__(1.0)		; vColor.a = 1f
		mov		dword [esp+StackSize+vColor+4], __?float32?__(0.0)		; vColor.r = 0f
		mov		dword [esp+StackSize+vColor+8], __?float32?__(1.0)		; vColor.g = 1f
		mov		dword [esp+StackSize+vColor+0Ch], __?float32?__(0.0)	; vColor.b = 0f
		
		; Set text color.
		lea		eax, [esp+StackSize+vColor]
		mov		ecx, _draw_string_set_primary_color
		call	ecx
		
		; Set draw string options.
		mov		dword [_g_rasterizer_draw_string_flags], 0					; _g_rasterizer_draw_string_flags
		mov		dword [_g_rasterizer_draw_string_style], 0FFFFFFFFh			; _g_rasterizer_draw_string_style
		mov		dword [_g_rasterizer_draw_string_justification], 0			; _g_rasterizer_draw_string_justification
		mov		dword [_g_rasterizer_draw_string_font], 6					; font?
		
		; FPS ----------------------------------------------------------------
		
		; Calculate fps.
		call	_Dbg_CalculateFps
		movss	dword [esp+StackSize+TempFloat], xmm0
		
		; Format fps string.
		fld		dword [esp+StackSize+TempFloat]
		sub		esp, 8
		fstp	qword [esp]
		push	Dbg_FpsCounterFormatString
		push	31
		lea		eax, [esp+StackSize+StringBuffer+10h]
		push	eax
		mov		eax, snwprintf
		call	eax
		add		esp, 4*5
		
		; Draw string to screen.
		lea		ecx, [esp+StackSize+StringBuffer]
		push	0
		push	0
		push	__?float32?__(1.0)
		push	0
		push	0
		push	0
		lea		eax, [esp+StackSize+Bounds+(6*4)]
		push	eax
		mov		eax, _rasterizer_draw_string
		call	eax
		
		; GPU Clk ------------------------------------------------------------
		
		; Update string bounds.
		mov		word [esp+StackSize+Bounds+RECT.upper], 120
		mov		word [esp+StackSize+Bounds+RECT.lower], 140
		
		; Calculate GPU clock speed.
		mov		eax, dword [0FD680500h]
		shr		eax, 8
		and		eax, 0FFh
		mov		dword [esp+StackSize+TempFloat], eax		; gpu_clk = *NV_PRAMDAC_NVPLL_COEFF_NDIV
		
		fild	dword [esp+StackSize+TempFloat]
		fmul	dword [Dbg_GpuClkMulDiv]
		fdiv	dword [Dbg_GpuClkMulDiv+4]					; gpu_clk = (gpu_clk * 16.666666666666) / 2
		
		; Format fps string.
		sub		esp, 8
		fstp	qword [esp]
		push	Dbg_GpuClkFormatString
		push	31
		lea		eax, [esp+StackSize+StringBuffer+10h]
		push	eax
		mov		eax, snwprintf
		call	eax
		add		esp, 4*5
		
		; Draw string to screen.
		lea		ecx, [esp+StackSize+StringBuffer]
		push	0
		push	0
		push	__?float32?__(1.0)
		push	0
		push	0
		push	0
		lea		eax, [esp+StackSize+Bounds+(6*4)]
		push	eax
		mov		eax, _rasterizer_draw_string
		call	eax
		
		; GPU Load -----------------------------------------------------------
		
		; Update string bounds.
		;mov		word [esp+StackSize+Bounds+RECT.upper], 140
		;mov		word [esp+StackSize+Bounds+RECT.lower], 160
		
		; Cleanup stack frame.
		pop		esi
		pop		ecx
		add		esp, StackStart
		ret
		
		align 4, db 0
		
		%undef StackStart
		%undef StackSize
		
	;---------------------------------------------------------
	; void Dbg_CalculateFps()
	;---------------------------------------------------------
_Dbg_CalculateFps:

		%define StackSize			0Ch
		%define StackStart			0h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	ecx
		push	edx
		push	edi
		
		; Calculate the starting index.
		mov		cx, word [_g_swap_count_array_index]
		movsx	eax, cx
		add		eax, 14
		cdq
		mov		edi, 15
		idiv	edi										; swapArrayIndex<edx> = (_g_swap_count_array_index + 14) % 15
		
		mov		edi, 1
		cvtsi2ss	xmm3, edi							; xmm3 = 1.0f
		xorps	xmm2, xmm2								; xmm2 = 0f
		movaps	xmm0, xmm2								; fSwapCounter = 0f
		movaps	xmm1, xmm2								; fVBlankCounter = 0f
		
.loop:

		; Calculate total number of swap calls and vblank intervals.
		movsx	eax, dx
		movsx	edx, word _g_swap_count_array [eax*2]
		add		eax, 14
		cvtsi2ss	xmm4, edx
		cdq
		mov		edi, 15
		idiv	edi										; swapArrayIndex<edx> = (swapArrayIndex + 14) % 15
		
		addss	xmm4, xmm0
		movaps	xmm0, xmm4								; fSwapCounter += (float)_g_swap_count_array[swapArrayIndex]
		addss	xmm1, xmm3								; fVBlankCounter += 1.0f
		cmp		dx, cx									; while (swapArrayIndex != _g_swap_count_array_index)
		jnz		.loop
		
		; Make sure the vblank counter isn't 0.
		comiss	xmm1, xmm2
		jz		.exit_fail
		
		; Calculate the average number of swaps per vblank.
		divss	xmm0, xmm1								; fAverageSwapsPerVBlank
		comiss	xmm0, xmm2								; if (fAverageSwapsPerVBlank <= 0f)
		jbe		.exit_fail
		
		; Get the refresh rate and make sure it's valid.
		movsx	eax, word [_g_refresh_rate_hz]
		test	eax, eax								; if (_g_refresh_rate_hz != 0)
		jnz		.calc_fps
		
			mov		eax, 60								; Default to 60 Hz if not set
		
.calc_fps:

		; Calculate final FPS.
		cvtsi2ss	xmm2, eax
		divss		xmm2, xmm0							; fps = (float)refreshRate / fAverageSwapsPerVBlank
		movss		xmm0, xmm2							; return fps
		jmp			.exit
		
.exit_fail:

		xorps	xmm0, xmm0								; return 0f
		
.exit:
		
		; Cleanup stack frame.
		pop		edi
		pop		edx
		pop		ecx
		add		esp, StackStart
		ret
		
		align 4, db 0
		
		%undef StackStart
		%undef StackSize
		
		
		
_Dbg_FpsCounterFormatString:
		dw __?utf16?__(`FPS: %.1f`),0
		align 4, db 0
		
_Dbg_GpuClkFormatString:
		dw __?utf16?__(`GPU Clk: %.2f Mhz`),0
		align 4, db 0
		
_Dbg_GpuLoadFormatString:
		dw __?utf16?__(`GPU Load: %.2f Mhz`),0
		align 4, db 0
		
_Dbg_GpuClkMulDiv:
		dd 16.666666666666
		dd 2.0
		
		