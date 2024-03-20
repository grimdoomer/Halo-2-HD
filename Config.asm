

;---------------------------------------------------------
; Config file definitions:
;---------------------------------------------------------
struc ConfigFileValue
	.NameCrc:		resd	1
	.Type:			resd	1
	.Value:			resd	1
endstruc

; Config file option types:
%define CFG_TYPE_INT		0		; Value is a signed integer
%define CFG_TYPE_FLOAT		1		; Value is a 32-bit float
%define CFG_TYPE_STRING		2		; Value is a pointer to a null terminated ascii string
%define CFG_TYPE_BOOL		3		; Value is a 32-bit integer with value 0 or 1, config file value must be either "false" or "true"

; CFG_OPTION <variable name> <name crc> <option type> <default value>
%macro CFG_OPTION 4

	istruc ConfigFileValue
		at .NameCrc,		dd %2
		at .Type,			dd %3
	%1: at .Value,			dd %4
	iend
	
%endmacro

%define Cfg_ConfigFileOptionCount		(_Cfg_ConfigFileOptionTableEnd - _Cfg_ConfigFileOptionTable) / ConfigFileValue_size


; Compilation options:
%define CFG_DEBUG_ENABLED			1


%if CFG_DEBUG_ENABLED != 0

HACK_DATA Cfg_DbgLineParseFormatString
HACK_DATA Cfg_DbgConfigOptionIntFormatString
HACK_DATA Cfg_DbgConfigOptionFloatFormatString
HACK_DATA Cfg_DbgConfigOptionStringFormatString

%endif


	;---------------------------------------------------------
	; Cfg_ParseConfigFile() -> Parse configuration file
	;---------------------------------------------------------
_Cfg_ParseConfigFile:

		%define StackSize				1Ch
		%define StackStart				1Ch
		%define BytesRead				-01Ch
		%define CfgFileHandle			-18h
		%define CfgFileSize				-14h
		%define CfgFileBuffer			-10h
		%define pBuffer					-0Ch
		%define pNameString				-8h
		%define pValueString			-4h
		
		; Setup the stack frame.
		sub		esp, StackStart
		
		; Initialize variables.
		mov		dword [esp+StackSize+CfgFileHandle], 0FFFFFFFFh
		mov		dword [esp+StackSize+CfgFileBuffer], 0
		
		; Open the config file for reading.
		push	0						; hTemplate = NULL
		push	80h						; FILE_ATTRIBUTE_NORMAL
		push	3						; OPEN_EXISTING
		push	0						; lpSecurityAttributes = NULL
		push	0						; no file share
		push	80000000h				; GENERIC_READ
		push	Cfg_ConfigFilePath		; file name
		mov		eax, CreateFileA
		call	eax						; CfgFileHandle = CreateFileA("D:\\hd_config.ini", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		cmp		eax, 0FFFFFFFFh			; if (CfgFileHandle == INVALID_HANDLE_VALUE)
		jz		_Cfg_ParseConfigFile_exit
		
		; Save the handle value.
		mov		dword [esp+StackSize+CfgFileHandle], eax
		
		; Get the size of the config file.
		push	0						; lpFileSizeHigh = NULL
		push	eax
		mov		eax, GetFileSize
		call	eax
		
		mov		dword [esp+StackSize+CfgFileSize], eax			; CfgFileSize = GetFileSize(CfgFileHandle, NULL);
		
		; Allocate a buffer to hold the config file data.
		lea		eax, [eax+1]
		push	eax												; CfgFileSize + 1 for null terminator
		mov		eax, malloc
		call	eax
		add		esp, 4
		cmp		eax, 0											; if (malloc(CfgFileSize + 1) == NULL)
		jz		_Cfg_ParseConfigFile_cleanup
		
		; Save the buffer address and null terminate it.
		mov		dword [esp+StackSize+CfgFileBuffer], eax		; CfgFileBuffer = ...
		mov		dword [esp+StackSize+pBuffer], eax				; pBuffer = ...
		add		eax, dword [esp+StackSize+CfgFileSize]
		mov		byte [eax], 0
		
		; Read the config file data.
		push	0
		lea		eax, [esp+StackSize+BytesRead]
		push	eax
		push	dword [esp+StackSize+CfgFileSize+8]
		push	dword [esp+StackSize+CfgFileBuffer+0Ch]
		push	dword [esp+StackSize+CfgFileHandle+10h]
		mov		eax, ReadFile
		call	eax
		cmp		eax, 0
		jz		_Cfg_ParseConfigFile_cleanup
		
		;INT3
		
_Cfg_ParseConfigFile_next_line:
		
		; Tokenize the next line.
		lea		eax, [esp+StackSize+pValueString]
		push	eax
		lea		eax, [esp+StackSize+pNameString+4]
		push	eax
		lea		eax, [esp+StackSize+pBuffer+8]
		push	eax
		call	_Cfg_TokenizeNextLine
		;INT3
		cmp		al, 0
		jz		_Cfg_ParseConfigFile_cleanup
		
		;INT3
		
		; Check if the line contained valid data or was empty.
		cmp		dword [esp+StackSize+pValueString], 0
		jz		_Cfg_ParseConfigFile_next_line
		
		;INT3
		
		; Parse the tokens.
		mov		eax, dword [esp+StackSize+pValueString]
		push	eax
		mov		eax, dword [esp+StackSize+pNameString+4]
		push	eax
		call	_Cfg_ParseConfigLine
		cmp		eax, 0
		jz		_Cfg_ParseConfigFile_cleanup
		
		; Check if there's more data to parse.
		cmp		dword [esp+StackSize+pBuffer], 0
		jnz		_Cfg_ParseConfigFile_next_line
		
_Cfg_ParseConfigFile_cleanup:

		;INT3

		; Check if the file buffer needs to be free'd.
		cmp		dword [esp+StackSize+CfgFileBuffer], 0
		jz		_Cfg_ParseConfigFile_close
		
			mov		eax, dword [esp+StackSize+CfgFileBuffer]
			push	eax
			mov		eax, free
			call	eax
			add		esp, 4
		
_Cfg_ParseConfigFile_close:

		; Check if the cfg file handle needs to be closed.
		cmp		dword [esp+StackSize+CfgFileHandle], 0FFFFFFFFh
		jz		_Cfg_ParseConfigFile_exit
		
			mov		eax, dword [esp+StackSize+CfgFileHandle]
			push	eax
			mov		eax, CloseHandle
			call	eax

_Cfg_ParseConfigFile_exit:

%if CFG_DEBUG_ENABLED != 0

		; Print the value of all the config options.
		call	_Cfg_PrintConfigOptions

%endif

		; Cleanup the stack frame.
		add		esp, StackStart
		ret
		
		%undef pValueString
		%undef pNameString
		%undef pBuffer
		%undef CfgFileBuffer
		%undef CfgFileSize
		%undef CfgFileHandle
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; bool Cfg_ParseConfigLine(char* pNameString, char* pValueString) -> Parses the tokens for a single line in the config file
	;---------------------------------------------------------
_Cfg_ParseConfigLine:

		%define StackSize				18h
		%define StackStart				0Ch
		%define pConfigOption			-0Ch
		%define Checksum				-8h
		%define IsValid					-4h
		%define pNameString				4h
		%define pValueString			8h
		
		; Setup the stack frame.
		sub		esp, StackStart
		push	esi
		push	ecx
		push	edx			; Trashed in atoi64
		
		mov		dword [esp+StackSize+Checksum], 0
		mov		dword [esp+StackSize+IsValid], 0
		
		; Calculate the length of the name string.
		mov		eax, dword [esp+StackSize+pNameString]
		push	eax
		mov		eax, lstrlenA								; length = lstrlenA(pNameString)
		call	eax
		
		; Calculate the crc of the config value name.
		push	eax											; length
		mov		eax, dword [esp+StackSize+pNameString+4]
		push	eax											; buffer
		lea		eax, [esp+StackSize+Checksum+8]
		push	eax											; &Checksum
		mov		eax, _crc32_calculate
		call	eax											; _crc32_calculate(&checksum, pNameString, length)
		add		esp, 0Ch
		
%if CFG_DEBUG_ENABLED != 0

		; Print the parsed line info.
		push	dword [esp+StackSize+pValueString]
		push	dword [esp+StackSize+pNameString+4]
		push	dword [esp+StackSize+Checksum+8]
		push	Cfg_DbgLineParseFormatString
		mov		eax, DbgPrint
		call	eax
		add		esp, 4*4

%endif
		
		xor		ecx, ecx									; i = 0
		mov		eax, dword [esp+StackSize+Checksum]
		mov		esi, Cfg_ConfigFileOptionTable
		
_Cfg_ParseConfigLine_loop:

		; Check if the current option crc matches what we calculated.
		cmp		dword [esi+ConfigFileValue.NameCrc], eax							; if (Cfg_ConfigFileOptionTable[i].NameCrc == Checksum)
		jz		_Cfg_ParseConfigLine_parse_value
		
		; Next config value entry.
		inc		ecx
		add		esi, ConfigFileValue_size
		cmp		ecx, Cfg_ConfigFileOptionCount										; while (++i < Cfg_ConfigFileOptionCount)
		jl		_Cfg_ParseConfigLine_loop
		
		; Config option with matching crc not found, return success as we don't want to fail out in case this
		; is an old version of the patch running against a new config file or something.
		jmp		_Cfg_ParseConfigLine_exit_success
		
_Cfg_ParseConfigLine_parse_value:

		; Check the config option type.
		mov		dword [esp+StackSize+pConfigOption], esi
		cmp		dword [esi+ConfigFileValue.Type], CFG_TYPE_INT						; if (pConfigOption->Type == CFG_TYPE_INT)
		jz		_Cfg_ParseConfigLine_parse_uint
		
		cmp		dword [esi+ConfigFileValue.Type], CFG_TYPE_FLOAT					; if (pConfigOption->Type == CFG_TYPE_FLOAT)
		jz		_Cfg_ParseConfigLine_parse_float
		
		cmp		dword [esi+ConfigFileValue.Type], CFG_TYPE_STRING					; else if (pConfigOption->Type == CFG_TYPE_STRING)
		jz		_Cfg_ParseConfigLine_parse_string
		
		cmp		dword [esi+ConfigFileValue.Type], CFG_TYPE_BOOL						; else if (pConfigOption->Type == CFG_TYPE_BOOL)
		jz		_Cfg_ParseConfigLine_parse_bool
		
		jmp		_Cfg_ParseConfigLine_exit
		
_Cfg_ParseConfigLine_parse_uint:

		; Parse the value as an integer, if the line ends with junk data it's fine as atoi will stop parsing
		; af the first non-numeric character.
		push	dword [esp+StackSize+pValueString]
		mov		eax, atoi64
		call	eax
		add		esp, 4
		mov		dword [esi+ConfigFileValue.Value], eax
		
		jmp		_Cfg_ParseConfigLine_exit_success
		
_Cfg_ParseConfigLine_parse_float:

		; Parse the value as a float, if the line ends with junk data it's fine as atoi will stop parsing
		; af the first non-numeric character.
		push	dword [esp+StackSize+pValueString]
		mov		eax, atof
		call	eax
		add		esp, 4
		movss	dword [esi+ConfigFileValue.Value], xmm0
		
		jmp		_Cfg_ParseConfigLine_exit_success
		
_Cfg_ParseConfigLine_parse_string:

		; Calculate the length of the string.
		push	dword [esp+StackSize+pValueString]
		mov		eax, lstrlenA
		call	eax
		push	eax											; Push length to the stack so we can retrieve it later
		inc		eax											; length = lstrlenA(pValueString) + 1 (for null temrinator)

		; Allocate a new buffer for the string.
		push	eax
		mov		eax, malloc
		call	eax											; pValueBuffer = malloc(length)
		add		esp, 4
		cmp		eax, 0										; if (pValueBuffer == NULL)
		jz		_Cfg_ParseConfigLine_exit
		
		; Copy the value string to the new buffer.
		mov		dword [esi+ConfigFileValue.Value], eax		; pConfigOption->Value = pValueBuffer
		push	dword [esp+StackSize+pValueString+4]
		push	eax
		mov		eax, lstrcpyA
		call	eax											; lstrcpyA(pValueBuffer, pValueString)
		
		; Null terminate the string.
		pop		eax											; Pop length from stack
		add		eax, dword [esi+ConfigFileValue.Value]
		mov		byte [eax], 0								; Null terminate the string
		
		jmp		_Cfg_ParseConfigLine_exit_success
		
_Cfg_ParseConfigLine_parse_bool:

		; Check if the value is 'true'.
		mov		eax, dword [esp+StackSize+pValueString]
		cmp		dword [eax], 'true'							; if (*(unsigned int*)pValueString == 'true')
		jnz		_Cfg_ParseConfigLine_parse_bool_false
		
		mov		dword [esi+ConfigFileValue.Value], 1		; pConfigOption->Value = TRUE
		jmp		_Cfg_ParseConfigLine_exit_success
		
_Cfg_ParseConfigLine_parse_bool_false:

		; Check if the value is 'fals'.
		cmp		dword [eax], 'fals'							; if (*(unsigned int*)pValueString == 'fals')
		jnz		_Cfg_ParseConfigLine_exit
		
		mov		dword [esi+ConfigFileValue.Value], 0		; pConfigOption->Value = FALSE
		jmp		_Cfg_ParseConfigLine_exit_success
		
_Cfg_ParseConfigLine_exit_success:

		mov		dword [esp+StackSize+IsValid], 1			; return true

_Cfg_ParseConfigLine_exit:
		
		; Cleanup the stack frame.
		mov		eax, dword [esp+StackSize+IsValid]
		pop		edx
		pop		ecx
		pop		esi
		add		esp, StackStart
		ret 8
		
		%undef pValueString
		%undef pNameString
		%undef IsValid
		%undef Checksum
		%undef pConfigOption
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; bool Cfg_TokenizeNextLine(char** ppBuffer, char** ppNameString, char** ppValueString) -> Tokenizes the next line of the config file
	;
	; 	Returns true if successful and false if a parsing error occured
	;---------------------------------------------------------
_Cfg_TokenizeNextLine:

		%define StackSize				10h
		%define StackStart				0Ch
		%define TempPtr					-8h
		%define IsValid					-4h
		%define ppBuffer				4h
		%define ppNameString			8h
		%define ppValueString			0Ch
		
		; Setup stack frame.
		sub		esp, StackStart
		push	esi
		
		; Initialize variables.
		mov		dword [esp+StackSize+IsValid], 0			; IsValid = false
		
		; Initialize out params.
		mov		eax, dword [esp+StackSize+ppNameString]
		mov		dword [eax], 0								; *ppNameString = NULL
		mov		eax, dword [esp+StackSize+ppValueString]
		mov		dword [eax], 0								; *ppValueString = NULL

		; Skip leading whitespace.
		mov		esi, dword [esp+StackSize+ppBuffer]
		push 	dword [esi]									; *ppBuffer
		call	_Cfg_SkipWhitespace
		mov		esi, eax
		cmp		esi, 0										; if ((pPtr = _Cfg_SkipWhitespace(pPtr)) == NULL) return NULL
		jz		_Cfg_TokenizeNextLine_exit
		
		; Check if this is a comment line.
		cmp		byte [esi], ';'								; if (*pPtr == ';')
		jz		_Cfg_TokenizeNextLine_empty_line
		cmp		byte [esi], `\r`							; if (*pPtr == `\r`)
		jz		_Cfg_TokenizeNextLine_empty_line
		cmp		byte [esi], `\n`							; if (*pPtr == `\n`)
		jz		_Cfg_TokenizeNextLine_empty_line
		
		jmp		_Cfg_TokenizeNextLine_continue
		
_Cfg_TokenizeNextLine_empty_line:
		
			; Line is a comment, skip to end of line.
			push	`\n`
			push	esi
			call	_Cfg_StrFind							; if ((pPtr = _Cfg_StrFind(pPtr, `\n`)) == NULL) return NULL
			;INT3
			mov		esi, eax
			cmp		esi, 0
			jz		_Cfg_TokenizeNextLine_exit
			
			; Save the next line pointer.
			inc		esi
			mov		eax, dword [esp+StackSize+ppBuffer]
			mov		dword [eax], esi						; *ppBuffer = pPtr + 1
			mov		dword [esp+StackSize+IsValid], 1		; return true
			jmp		_Cfg_TokenizeNextLine_exit
		
_Cfg_TokenizeNextLine_continue:
		
		; Save the name pointer.
		mov		eax, dword [esp+StackSize+ppNameString]
		mov		dword [eax], esi							; *ppNameString = pPtr
		
		;INT3
		
_Cfg_TokenizeNextLine_name_loop:

		; Check if the next character is alpha numeric.
		mov		al, byte [esi]
		push	eax
		call	_Cfg_IsAlphaNumeric
		cmp		al, 0										; if (_Cfg_IsAlphaNumeric(*pPtr) == false) break
		jz		_Cfg_TokenizeNextLine_name_loop_end
		
		; Next character.
		inc		esi											; pPtr++
		jmp		_Cfg_TokenizeNextLine_name_loop
		
_Cfg_TokenizeNextLine_name_loop_end:

		; Save the end of name pointer.
		mov		dword [esp+StackSize+TempPtr], esi
		
		; Find the string split character.
		push	'='
		push	esi
		call	_Cfg_StrFind
		;INT3
		mov		esi, eax
		cmp		esi, 0
		jz		_Cfg_TokenizeNextLine_exit
		
		; Null terminate the name string.
		mov		eax, dword [esp+StackSize+TempPtr]
		mov		byte [eax], 0
		inc		esi

		; Skip any whitespace characters.
		push	esi
		call	_Cfg_SkipWhitespace
		;INT3
		mov		esi, eax
		cmp		esi, 0
		jz		_Cfg_TokenizeNextLine_exit
		
		; Save the value pointer.
		mov		eax, dword [esp+StackSize+ppValueString]
		mov		dword [eax], esi
		
		; At this point we consider the string valid, if the newline character isn't found it could mean
		; this is the last line in the config file, in which case we don't want to fail out.
		mov		dword [esp+StackSize+IsValid], 1
		
		; Find the end of the line.
		push	`\n`
		push	esi
		call	_Cfg_StrFind
		
		;INT3
		mov		esi, eax
		mov		eax, dword [esp+StackSize+ppBuffer]
		mov		dword [eax], esi
		
		cmp		esi, 0
		jz		_Cfg_TokenizeNextLine_exit
		
		; Null terminate the value string.
		mov		byte [esi], 0
		inc		esi
		mov		eax, dword [esp+StackSize+ppBuffer]
		mov		dword [eax], esi
		
_Cfg_TokenizeNextLine_exit:
		
		; Cleanup stack frame.
		mov		eax, dword [esp+StackSize+IsValid]
		pop		esi
		add		esp, StackStart
		ret 0Ch
		
		%undef ppValueString
		%undef ppNameString
		%undef ppBuffer
		%undef IsValid
		%undef TempPtr
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; char* Cfg_SkipWhitespace(char* psBuffer) -> Skips leading whitespace characters and returns the address of the first valid character or NULL if null terminator was reached
	;---------------------------------------------------------
_Cfg_SkipWhitespace:

		%define psBuffer		4h
		
		push	esi
		mov		esi, dword [esp+4+psBuffer]

_Cfg_SkipWhitespace_loop:

		; Check the next character.
		mov		al, byte [esi]
		cmp		al, ' '							; if (*psBuffer == ' ')
		jz		_Cfg_SkipWhitespace_next
		
		cmp		al, `\t`						; if (*psBuffer == '\t')
		jz		_Cfg_SkipWhitespace_next
		
		cmp		al, 0							; if (*psBuffer != 0) return psBuffer
		jnz		_Cfg_SkipWhitespace_done
		
		; Null terminator reached.
		mov		esi, 0							; return NULL
		jmp		_Cfg_SkipWhitespace_done
		
_Cfg_SkipWhitespace_next:

		; Next character.
		inc		esi
		jmp		_Cfg_SkipWhitespace_loop
		
_Cfg_SkipWhitespace_done:

		mov		eax, esi
		pop		esi
		ret 4
		
		%undef psBuffer
		
		align 4, db 0
		
	;---------------------------------------------------------
	; bool Cfg_IsAlphaNumeric(char character) -> Returns true if the character is alpha numeric, false otherwise
	;---------------------------------------------------------
_Cfg_IsAlphaNumeric:

		%define StackSize		4h
		%define character		4h
		
		push	ecx
		xor		eax, eax
		
		; Check if numeric.
		mov		cl, byte [esp+StackSize+character]
		cmp		cl, '9'
		jg		_Cfg_IsAlphaNumeric_check_upper
		
		cmp		cl, '0'
		jl		_Cfg_IsAlphaNumeric_exit
		
		; Character is numeric
		mov		al, 1
		jmp		_Cfg_IsAlphaNumeric_exit
		
_Cfg_IsAlphaNumeric_check_upper:

		; Check if upper case alphabetic.
		cmp		cl, 'Z'
		jg		_Cfg_IsAlphaNumeric_check_lower
		
		cmp		cl, 'A'
		jl		_Cfg_IsAlphaNumeric_exit
		
		; Character is upper case alphabetic.
		mov		al, 1
		jmp		_Cfg_IsAlphaNumeric_exit
		
		
_Cfg_IsAlphaNumeric_check_lower:

		; Check if lower case alphabetic.
		cmp		cl, 'z'
		jg		_Cfg_IsAlphaNumeric_exit
		
		cmp		cl, 'a'
		jl		_Cfg_IsAlphaNumeric_exit
		
		; Character is lower case alphabetic.
		mov		al, 1

_Cfg_IsAlphaNumeric_exit:

		pop		ecx
		ret 4
		
		%undef character
		%undef StackSize
		
		align 4, db 0
		
	;---------------------------------------------------------
	; char* Cfg_StrFind(char* pString, char find)
	;---------------------------------------------------------
_Cfg_StrFind:

		%define StackStart		4h
		%define pString			4h
		%define find			8h
		
		push	esi
		mov		esi, dword [esp+StackStart+pString]
		
_Cfg_StrFind_loop:
		
		; Check if this is the character we're looking for.
		mov		al, byte [esi]
		cmp		al, byte [esp+StackStart+find]				; if (*pString == find) return pString
		jz		_Cfg_StrFind_exit
		
		; Check if we hit the end of the line.
		cmp		al, 0										; if (*pString == NULL) return NULL
		jz		_Cfg_StrFind_fail
		cmp		al, `\n`									; if (*pString == `\n`) return NULL
		jz		_Cfg_StrFind_fail
		
		; Next character.
		inc		esi
		jmp		_Cfg_StrFind_loop
		
_Cfg_StrFind_fail:

		xor		esi, esi
		
_Cfg_StrFind_exit:

		mov		eax, esi
		pop		esi
		ret 8
		
		%undef find
		%undef pString
		%undef StackStart
		
		align 4, db 0
		
%if CFG_DEBUG_ENABLED != 0
		
	;---------------------------------------------------------
	; Cfg_PrintConfigOptions() -> Print all the config options and their values
	;---------------------------------------------------------
_Cfg_PrintConfigOptions:

		%define StackSize			8h
		%define StackStart			0h
		
		; Setup stack frame.
		sub		esp, StackStart
		push	esi
		push	edi
		
		mov		edi, Cfg_ConfigFileOptionCount
		mov		esi, Cfg_ConfigFileOptionTable
		
_Cfg_PrintConfigOptions_loop:

		; RIP: DbgPrint doesn't support float...
		; Float is a special case.
		;cmp		dword [esi+ConfigFileValue.Type], CFG_TYPE_FLOAT
		;jz		_Cfg_PrintConfigOptions_loop_float

		push	dword [esi+ConfigFileValue.Value]
		push	dword [esi+ConfigFileValue.NameCrc]
		cmp		dword [esi+ConfigFileValue.Type], CFG_TYPE_STRING
		jz		_Cfg_PrintConfigOptions_loop_string
		
		push	Cfg_DbgConfigOptionIntFormatString
		jmp		_Cfg_PrintConfigOptions_loop_print
		
_Cfg_PrintConfigOptions_loop_string:

		push	Cfg_DbgConfigOptionStringFormatString

_Cfg_PrintConfigOptions_loop_print:

		mov		eax, DbgPrint
		call	eax
		add		esp, 3*4
		jmp		_Cfg_PrintConfigOptions_loop_next
		
_Cfg_PrintConfigOptions_loop_float:

		INT3
		sub		esp, 8
		fld		dword [esi+ConfigFileValue.Value]
		fstp	qword [esp]
		push	dword [esi+ConfigFileValue.NameCrc]
		push	Cfg_DbgConfigOptionFloatFormatString
		mov		eax, DbgPrint
		call	eax
		add		esp, 4*4
		
_Cfg_PrintConfigOptions_loop_next:

		; Next option.
		add		esi, ConfigFileValue_size
		dec		edi
		jnz		_Cfg_PrintConfigOptions_loop
		
		; Cleanup stack frame.
		pop		edi
		pop		esi
		add		esp, StackStart
		ret
		
		%undef StackStart
		%undef StackSize
		
		align 4, db 0
		
%endif ; CFG_DEBUG_ENABLED
		
		

; Config file options:
_Cfg_ConfigFileOptionTable:

	; Test options:
	;CFG_OPTION _Cfg_TestIntValue1, 0x5acc6ccd, CFG_TYPE_INT, 0
	;CFG_OPTION _Cfg_TestIntValue2, 0xc3c53d77, CFG_TYPE_INT, 0
	;CFG_OPTION _Cfg_TestIntValue3, 0xb4c20de1, CFG_TYPE_INT, 0
	;CFG_OPTION _Cfg_TestStringValue, 0xf70a8ca7, CFG_TYPE_STRING, 0
	;CFG_OPTION _Cfg_TestBoolValueTrue, 0x36adb06d, CFG_TYPE_BOOL, 0
	;CFG_OPTION _Cfg_TestBoolValueFalse, 0x590e3439, CFG_TYPE_BOOL, 0
	
	; Video options:
	CFG_OPTION _Cfg_Enable1080iSupport, 0x4b51a48c, CFG_TYPE_BOOL, 0
	
	; Rendering options:
	CFG_OPTION _Cfg_DisableAnamorphicScaling, 0xc6f838a8, CFG_TYPE_BOOL, 1
	CFG_OPTION _Cfg_DisableAtmosphericFog, 0xf02ad616, CFG_TYPE_BOOL, 0
	
	; Gameplay options:
	CFG_OPTION _Cfg_FieldOfView, 0x6ab10407, CFG_TYPE_FLOAT, __?float32?__(70.0)
	CFG_OPTION _Cfg_SplitScreenFavor, 0xc156d0a4, CFG_TYPE_INT, 2
	
	; Misc options:
	CFG_OPTION _Cfg_DebugMode, 0x611da1be, CFG_TYPE_BOOL, 0
	
	; Overclocking options:
	CFG_OPTION _Cfg_OverclockGPU, 0xb0f1b6e5, CFG_TYPE_BOOL, 0
	CFG_OPTION _Cfg_GPUOverclockStep, 0xdc90cddb, CFG_TYPE_INT, 28
	CFG_OPTION _Cfg_OverrideFanSpeed, 0x40f0721b, CFG_TYPE_BOOL, 0
	CFG_OPTION _Cfg_FanSpeedPercent, 0xc6c92d4d, CFG_TYPE_INT, 100

_Cfg_ConfigFileOptionTableEnd:

_Cfg_ConfigFilePath:
		db `D:\\hd_config.ini`,0
		align 4, db 0
		
%if CFG_DEBUG_ENABLED != 0
		
_Cfg_DbgLineParseFormatString:
		db `Cfg: Crc=0x%08x Name=%s Value=%s\n`,0
		align 4, db 0

_Cfg_DbgConfigOptionIntFormatString:
		db `Cfg: Crc=0x%08x Value=%d\n`,0
		align 4, db 0
		
_Cfg_DbgConfigOptionFloatFormatString:
		db `Cfg: Crc=0x%08x Value=%f\n`,0
		align 4, db 0
	
_Cfg_DbgConfigOptionStringFormatString:
		db `Cfg: Crc=0x%08x Value=%s\n`,0
		align 4, db 0
		
%endif ; CFG_DEBUG_ENABLED
		
		