
.686p
.XMM
.model flat, C

.code

EXTERN crc_checksum_buffer : PROC

crc32_calculate PROC

    StackSize =     0Ch
    StackStart =    0h
    Checksum =      4h
    Buffer =        8h
    _Length =        0Ch
        
    ; Setup stack frame.
    sub     esp, StackStart
    push    edi
    push    ecx
    push    edx     ; trashed by _crc32_calculate
        
    ; Call fastcall version of _crc32_calculate.
    mov     edi, dword ptr [esp+StackSize+_Length]
    mov     eax, dword ptr [esp+StackSize+Buffer]
    push    dword ptr [esp+StackSize+Checksum]
    mov     ecx, crc_checksum_buffer
    call    ecx
        
    ; Cleanup stack frame.
    pop     edx
    pop     ecx
    pop     edi
    add     esp, StackStart
    ret

crc32_calculate ENDP

END