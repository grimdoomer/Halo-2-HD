
%include "DirectX_h.asm"


%ifdef D3DINC_IDirect3DDevice8_GetBackBuffer

;---------------------------------------------------------
; D3DSurface IDirect3DDevice8_GetBackBuffer(int BackBuffer)
;---------------------------------------------------------
_IDirect3DDevice8_GetBackBuffer:

		; Internal d3d device pointer must be defined.
		%ifndef D3D_g_pDevice
			%error "D3D_g_pDevice must be defined to call IDirect3DDevice8_GetBackBuffer"
		%endif
		
		%ifndef D3D_g_pDevice
			%error "D3DResource_AddRef must be defined to call IDirect3DDevice8_GetBackBuffer"
		%endif
		
		;FUNC_REQ D3D_g_pDevice, IDirect3DDevice8_GetBackBuffer
		;FUNC_REQ D3DResource_AddRef, IDirect3DDevice8_GetBackBuffer

		%define StackSize			8h
		%define StackStart			0h
		%define BackBuffer			4h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	esi
		push	ecx
		
		mov		ecx, dword [D3D_g_pDevice]
		
		; Check if caller is requesting a back buffer or front buffer.
		mov		eax, dword [esp+StackSize+BackBuffer]
		cmp		eax, 0FFFFFFFFh
		jnz		.get_back_buffer
		
			; Get the front buffer.
			mov		eax, 1
			mov		esi, dword [ecx+eax*4+1A14h]
			push	esi
			FAR_CALL	D3DResource_AddRef
			mov		eax, esi
			jmp		.exit
	
.get_back_buffer:

		; Get the specified back buffer.
		neg		eax
		sbb		eax, eax
		and		eax, 2
		mov		esi, dword [ecx+eax*4+1A14h]
		push	esi
		FAR_CALL	D3DResource_AddRef
		mov		eax, esi

.exit:

		; Cleanup stack frame.
		pop		ecx
		pop		esi
		add		esp, StackStart
		ret 4
		
		align 4, db 0
		
		%undef BackBuffer
		%undef StackStart
		%undef StackSize

%endif

%ifdef D3DINC_IDirect3DDevice8_GetTile

;---------------------------------------------------------
; void IDirect3DDevice8_GetTile(int index, D3DTILE* pTile)
;---------------------------------------------------------
_IDirect3DDevice8_GetTile:

		; Internal d3d device pointer must be defined.
		%ifndef D3D_g_pDevice
			%error "D3D_g_pDevice must be defined to call IDirect3DDevice8_GetTile"
		%endif
		
		;FUNC_REQ D3D_g_pDevice, IDirect3DDevice8_GetTile
		
		%define StackSize			8h
		%define StackStart			0h
		%define Index				4h
		%define pTile				8h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	ecx
		push	edx
		
		; Get a pointer to the d3d tile structure.
		mov		ecx, dword [D3D_g_pDevice]
		mov		eax, dword [esp+StackSize+Index]
		lea		eax, [eax+eax*2]
		lea		ecx, [ecx+eax*8+1AC0h]
		
		; Copy structure.
		mov		edx, dword [esp+StackSize+pTile]
		mov		eax, dword [ecx+D3DTILE.Flags]
		mov		dword [edx+D3DTILE.Flags], eax
		mov		eax, dword [ecx+D3DTILE.pMemory]
		mov		dword [edx+D3DTILE.pMemory], eax
		mov		eax, dword [ecx+D3DTILE.Size]
		mov		dword [edx+D3DTILE.Size], eax
		mov		eax, dword [ecx+D3DTILE.Pitch]
		mov		dword [edx+D3DTILE.Pitch], eax
		mov		eax, dword [ecx+D3DTILE.ZStartTag]
		mov		dword [edx+D3DTILE.ZStartTag], eax
		mov		eax, dword [ecx+D3DTILE.ZOffset]
		mov		dword [edx+D3DTILE.ZOffset], eax
		
		; Cleanup stack frame.
		pop		edx
		pop		ecx
		add		esp, StackStart
		ret 8
		
		align 4, db 0
		
		%undef pTile
		%undef Index
		%undef StackStart
		%undef StackSize

%endif



