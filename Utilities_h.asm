
%ifndef _UTILITIES_H
%define _UTILITIES_H

; Macro to compile in a utility function that may not exist in the binary or may have been inlined.
;
; UTIL_INCLUDE <function name>
%macro UTIL_INCLUDE 1
	%define UTILINC_%1
	HACK_FUNCTION %1
%endmacro

; Used to throw a compilation error if a required declaration is not defined.
;
; FUNC_REQ <define> <function name>
%macro FUNC_REQ 2
	%ifndef %1
		%error %1 must be defined to call %2
	%endif
%endmacro

; If the hacks segment is being used define some helper macros for calculating absolute addreses within it.
%ifdef HacksSegmentAddress

	%macro HACK_FUNCTION 1
		%define %1			HacksSegmentAddress + (_%1 - _hacks_code_start)
	%endmacro

	%macro HACK_DATA 1
		%define %1			HacksSegmentAddress + (_%1 - _hacks_code_start)
	%endmacro
	
	; HOOK_FUNCTION <address> <detour>		must be called from within .hacks segment!!!!
	%macro HOOK_FUNCTION 2
		push	%2
		push	%1
		call	_Util_InstallHook
	%endmacro

%endif ; HacksSegmentAddress

; Macro for triggering a breakpoint:
%macro INT3 0
	db 0CCh
%endmacro

; Macro for making a non-relative function call, overwrites eax:
; FAR_CALL <function address>
%macro FAR_CALL 1
		mov		eax, %1
		call	eax
%endmacro

; Macro for validating the expected size of a structure:
;
; ASSERT_STRUCT_SIZE <struct symbol name> <expected size>
%macro ASSERT_STRUCT_SIZE 2
	%if %1_size != %2
		%error %1 incorrect struct size, %eval(%1_size) != %num(%2, -1, 10)
	%endif
%endmacro

; Macro for clamping a register to a range:
;
; CLAMP <register>, <lower>, <upper>
%macro CLAMP 3
	
	; if (val < lower)
	cmp		%1, %2
	jge		%%upper
	
		; val = lower
		mov		%1, %2
	
%%upper:

	; if (val > upper)
	cmp		%1, %3
	jle		%%done
	
		; val = upper
		mov		%1, %3
		
%%done:
	
%endmacro

; Macro for port data output:
;
; OUTP <address>, <value>
%macro OUTP 2

	mov		dx, %1
	mov		al, %2
	out		dx, al

%endmacro

; Macro for port data input:
;
; INP <address>
%macro INP 1

	mov		dx, %1
	in		al, dx

%endmacro


; Functions implemented in Utilities.asm:
HACK_FUNCTION Util_InstallHook
HACK_FUNCTION Util_WriteDword
HACK_FUNCTION Util_ResolveKernelImports
HACK_FUNCTION Util_GetMemoryCapacity
HACK_FUNCTION Util_SetFanSpeed
HACK_FUNCTION Util_OverclockGPU
HACK_FUNCTION Util_DegreesToRadians
HACK_FUNCTION Util_AtaIdentify
HACK_FUNCTION Util_HddSetTransferSpeed

%endif ; _UTILITIES_H