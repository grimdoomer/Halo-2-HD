
%include "Utilities_h.asm"


%ifdef UTILINC_lstrcpyA

;---------------------------------------------------------
; void lstrcpyA(char* dst, char* src)
;---------------------------------------------------------
_lstrcpyA:

		%define StackSize		8h
		%define StackStart		0h
		%define Dst				4h
		%define Src				8h
		
		push	esi
		push	edi
		
		mov		esi, dword [esp+StackSize+Src]
		mov		edi, dword [esp+StackSize+Dst]
		
.loop:
		mov		al, byte [esi]
		mov		byte [edi], al
		inc		esi
		inc		edi
		test	al, al
		jnz		.loop
		
		pop		edi
		pop		esi
		ret 8
		
		align 4, db 0
		
		%undef Src
		%undef Dst
		%undef StackStart
		%undef StackSize
		
%endif

%ifdef UTILINC_atof

;---------------------------------------------------------
; double __cdecl atof(const char* str)
;---------------------------------------------------------
_atof:

		; Modified version of: https://github.com/GaloisInc/minlibc/blob/master/atof.c
		; Does not support exponent notation...

		%define StackSize		10h
		%define StackStart		4h
		%define TempFloat		-4
		%define Str				4h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	ecx
		push	edx
		push	edi

		mov		edi, dword [esp+StackSize+Str]

		mov     al, byte [edi]
        add     edi, 1
        lea     ecx, [eax - 48]
        xorps   xmm0, xmm0
        cmp     cl, 9
        ja      .LBB0_3
        xorps   xmm0, xmm0
		
		mov		dword [esp+StackSize+TempFloat], __?float32?__(10.0)
        movss   xmm1, dword [esp+StackSize+TempFloat]
		
.LBB0_2:                                ; =>This Inner Loop Header: Depth=1
        movzx   eax, al
        add     eax, -48
        xorps   xmm2, xmm2
        cvtsi2ss        xmm2, eax
        mulss   xmm0, xmm1
        addss   xmm0, xmm2
        movzx   eax, byte [edi]
        add     edi, 1
        lea     ecx, [eax - 48]
        cmp     cl, 10
        jb      .LBB0_2
		
.LBB0_3:
        cmp     al, 46
        jne     .LBB0_17
        mov     cl, byte [edi]
        lea     eax, [ecx - 48]
        cmp     al, 9
        ja      .LBB0_17
        add     edi, 1
        xor     eax, eax
        
		mov		dword [esp+StackSize+TempFloat], __?float32?__(10.0)
        movss   xmm1, dword [esp+StackSize+TempFloat]
		
.LBB0_6:                                ; =>This Inner Loop Header: Depth=1
        movzx   ecx, cl
        add     ecx, -48
        xorps   xmm2, xmm2
        cvtsi2ss        xmm2, ecx
        mulss   xmm0, xmm1
        addss   xmm0, xmm2
        add     eax, -1
        movzx   ecx, byte [edi]
        lea     edx, [ecx - 48]
        add     edi, 1
        cmp     dl, 10
        jb      .LBB0_6
        lea     ecx, [eax + 1]
        cmp     ecx, 2
        jl      .LBB0_8
		
.LBB0_18:                               ; =>This Inner Loop Header: Depth=1
        mulss   xmm0, xmm1
        add     ecx, -1
        cmp     ecx, 1
        ja      .LBB0_18
        jmp     .LBB0_17
		
.LBB0_8:
        test    eax, eax
        je      .LBB0_17
        mov     edx, eax
        neg     edx
        test    dl, 7
        je      .LBB0_10
        mov     dl, 1
        sub     dl, cl
        movzx   ecx, dl
        and     ecx, 7
        neg     ecx
        xor     edx, edx
        
		mov		dword [esp+StackSize+TempFloat], __?float32?__(0.1)
        movss   xmm1, dword [esp+StackSize+TempFloat]
		
.LBB0_12:                               ; =>This Inner Loop Header: Depth=1
        mulss   xmm0, xmm1
        add     edx, -1
        cmp     ecx, edx
        jne     .LBB0_12
        mov     ecx, eax
        sub     ecx, edx
        add     eax, 7
        jae     .LBB0_15
        jmp     .LBB0_17
		
.LBB0_10:
        mov     ecx, eax
        add     eax, 7
        jb      .LBB0_17
		
.LBB0_15:
        mov		dword [esp+StackSize+TempFloat], __?float32?__(0.1)
        movss   xmm1, dword [esp+StackSize+TempFloat]
		
.LBB0_16:                               ; =>This Inner Loop Header: Depth=1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        mulss   xmm0, xmm1
        add     ecx, 8
        jne     .LBB0_16
		
.LBB0_17:
		; Cleanup stack frame.
		pop		edi
		pop		edx
		pop		ecx
		add		esp, StackStart
        ret
		
		align 4, db 0
		
		%undef Str
		%undef StackStart
		%undef StackSize
		
%endif ; UTILINC_atof

;---------------------------------------------------------
; void Util_InstallHook(void *Address, void *Detour) -> Hooks at the specified address
;---------------------------------------------------------
_Util_InstallHook:

		%define StackSize		14h
		%define StackStart		8h
		%define ShellCode		-8
		%define Address			4h
		%define Detour			8h

		; Setup the stack frame.
		sub		esp, StackStart
		push	ecx
		push	edi
		push	esi

		; Setup the shellcode buffer.
		;	push	Address
		;	ret
		lea		esi, [esp+StackSize+ShellCode]
		mov		byte [esi], 68h
		mov		eax, dword [esp+StackSize+Detour]
		mov		dword [esi+1], eax
		mov		byte [esi+5], 0C3h

		; Setup for the memcpy operation.
		mov		edi, dword [esp+StackSize+Address]
		mov		ecx, 6

		; Disable write protect.
		pushf
		cli							; Disable interrupts
		mov		eax, cr0			; Get the control register value
		push	eax					; Save it for later
		and		eax, 0FFFEFFFFh		; Disable write-protect
		mov		cr0, eax

		; Copy the hook shellcode to the target address.
		cld
		rep		movsb

		; Re-enable write-protect.
		pop		eax
		mov		cr0, eax			; Re-enable write-protect
		popf

		; Destroy the stack frame and return.
		pop		esi
		pop		edi
		pop		ecx
		add		esp, StackStart
		ret		8

		align 4, db 0

		%undef Detour
		%undef Address
		%undef StackStart
		%undef StackSize
		
;---------------------------------------------------------
; void Util_WriteDword(void *Address, int value) -> Writes value at Address
;---------------------------------------------------------
_Util_WriteDword:

		%define StackSize		8h
		%define Address			4h
		%define Value			8h
		
		; Setup stack frame.
		push	esi
		push	edi
		
		mov		edi, dword [esp+StackSize+Address]
		mov		esi, dword [esp+StackSize+Value]
		
		; Disable write protect.
		pushf
		cli							; Disable interrupts
		mov		eax, cr0			; Get the control register value
		push	eax					; Save it for later
		and		eax, 0FFFEFFFFh		; Disable write-protect
		mov		cr0, eax
		
		; Write data.
		mov		dword [edi], esi
		
		; Re-enable write-protect.
		pop		eax
		mov		cr0, eax			; Re-enable write-protect
		popf
		
		; Cleanup the stack frame.
		pop		edi
		pop		esi
		ret 8
		
		align 4, db 0
		
		%undef Value
		%undef Address
		%undef StackSize
		
;---------------------------------------------------------
; void Util_ResolveKernelImports() -> resolves new kernel imports
;---------------------------------------------------------
_Util_ResolveKernelImports:

		; Make sure the kernel imports list is defined. The list should be a list of 32bit integers each representing
		; the ordinal of the exported kernel function to resolve. The list should have a "null terminator" entry with
		; the value 0 to denote the end of the list.
		;
		; Example:
		;	_Util_KernelImports:
		;		_HalReadWritePCISpace				dd 46
		;											dd 0
		%ifndef Util_KernelImports
			%error "Util_KernelImports must be defined to call Util_ResolveKernelImports()"
		%endif

		%define StackSize		14h
		%define StackStart		0h

		; Setup the stack frame.
		sub		esp, StackStart
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi

		; Get the address of the kernel export table
		mov		esi, 80010000h		; base kernel address
		mov		eax, [esi+3Ch]		; pKernelImg->PeOffset
		mov		ebx, [esi+eax+78h]	; pOptionalHeader->ExportsDirectory.VirtualAddress
		add		ebx, esi			; add kernel base address
		mov		edx, [ebx+1Ch]		; pExportsDirectory->AddressOfFunctions
		add		edx, esi			; address of the exports table

		; Get the address of our "import" table
		mov		edi, Util_KernelImports

.resolve_import:

		; Load the next import index and check if it is valid
		mov		ecx, [edi]			; ordinal = *Hack_KernelImports
		test	ecx, ecx			; if (ordinal == 0)
		jz		_Util_ResolveKernelImports_exit

		; Load the function address from the export table
		xor		eax, eax
		sub		ecx, [ebx+10h]		; ordinal -= pExportsDirectory->Base
		cmp		ecx, [ebx+14h]		; if (ordinal >= pExportsDirectory->NumberOfFunctions)
		jge		.empty
		
		mov		eax, [edx+4*ecx]	; Load the function offset from the export table
		test	eax, eax
		jz		.empty
		add		eax, esi			; Add the kernel base address to the function offset
		
.empty:
		mov		[edi], eax

		; Next import
		add		edi, 4
		jmp		.resolve_import

_Util_ResolveKernelImports_exit:

		; Destroy the stack frame and return.
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		add		esp, StackStart
		ret

		align 4, db 0

		%undef StackStart
		%undef StackSize
		
;---------------------------------------------------------
; void Util_GetMemoryCapacity()
;---------------------------------------------------------
_Util_GetMemoryCapacity:

		; HalReadWritePCISpace must be defined.
		%ifndef HalReadWritePCISpace
			%error "HalReadWritePCISpace kernel import must be declared and resolved to call Util_GetMemoryCapacity()"
		%endif

		%define StackSize			4h
		%define StackStart			4h
		%define MemoryTop			-4h
		
		; Setup stack frame.
		sub		esp, StackStart
		
		mov		dword [esp+StackSize+MemoryTop], 0
		
		; Query the host bridge for memory capacity value.
		lea		eax, [esp+StackSize+MemoryTop]
		push	0							; read
		push	1							; sizeof(MemoryTop)
		push	eax							; &MemoryTop
		push	87h							; CPU_MEMTOP_LIMIT
		push	0							; SlotNumber = host bridge
		push	0							; BusNumber
		call	dword [HalReadWritePCISpace]
		
		; Convert the memory top limit to number of MB present.
		movsx	eax, byte [esp+StackSize+MemoryTop]
		inc		eax
		imul	eax, 16						; CapacityInMB = (MemoryTop + 1) * 16
		
		; Cleanup stack frame.
		add		esp, StackStart
		ret
		
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
;---------------------------------------------------------
; void Util_SetFanSpeed(int percent)
;---------------------------------------------------------
_Util_SetFanSpeed:

		; HalWriteSMBusValue must be defined.
		%ifndef HalWriteSMBusValue
			%error "HalWriteSMBusValue kernel import must be declared and resolved to call Util_SetFanSpeed()"
		%endif
		
		; KeStallExecutionProcessor must be defined.
		%ifndef KeStallExecutionProcessor
			%error "KeStallExecutionProcessor kernel import must be declared and resolved to call Util_SetFanSpeed()"
		%endif
		
		%define StackSize			0h
		%define StackStart			0h
		%define Percent				4h
		
		; Setup stack frame.
		sub		esp, StackStart
		
		; Map the fan speed percent from [0, 100] -> [0, 50]
		mov		eax, dword [esp+StackSize+Percent]
		shr		eax, 1
		
		; Set the custom fan speed value.
		push	eax								; fan speed
		push	0
		push	6								; SMC_COMMAND_REQUEST_FAN_SPEED
		push	20h								; SMC_SLAVE_ADDRESS
		call	dword [HalWriteSMBusValue]
		
		; Give the SMC a chance to process the message, if not it can panic.
		push	100*1000						; 100ms
		call	dword [KeStallExecutionProcessor]
		
		; Set SMC fan mode to use custom speed.
		push	1								; SMC_FAN_OVERRIDE_USE_REQUESTED_FAN_SPEED
		push	0
		push	5								; SMC_COMMAND_FAN_OVERRIDE
		push	20h								; SMC_SLAVE_ADDRESS
		call	dword [HalWriteSMBusValue]
		
		; Cleanup stack frame.
		add		esp, StackStart
		ret 4
		
		%undef Percent
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
;---------------------------------------------------------
; void Util_OverclockGPU(int step)
;---------------------------------------------------------
_Util_OverclockGPU:

		%define StackSize			4h
		%define StackStart			0h
		%define Step				4h

		; Setup stack frame.
		sub		esp, StackStart
		push	esi

		; Get the NVPLL_COEFF value.
		mov		esi, dword [0FD680500h]
		
		; Mask out the old NDIV value.
		and		esi, ~0FF00h
		
		; Mask in the new NDIV value from the config file.
		mov		eax, dword [esp+StackSize+Step]
		and		eax, 0FFh
		shl		eax, 8
		or		esi, eax
		
		; Write the new NVPLL_COEFF value.
		mov		dword [0FD680500h], esi
		
		; Cleanup stack frame.
		pop		esi
		add		esp, StackStart
		ret 4
		
		%undef Step
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
;---------------------------------------------------------
; void Util_DegreesToRadians(float value)
;---------------------------------------------------------
_Util_DegreesToRadians:

		%define Value		4h
		
		movss	xmm0, dword [esp+Value]
		push	__?float32?__(0.01745)
		movss	xmm1, dword [esp]
		mulss	xmm0, xmm1					; return value * 0.01745f
		
		add		esp, 4
		ret 4
		
		align 4, db 0
		
		%undef Value
		
		
		
		
		
		