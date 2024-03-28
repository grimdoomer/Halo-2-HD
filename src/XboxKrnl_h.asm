
%ifndef _XBOXKRNL_H
%define _XBOXKRNL_H

%include "Utilities_h.asm"

;------------------------------------------------------------------------------
; General kernel definitions:

struc ANSI_STRING
    .Length             resw 1  ; USHORT
    .MaximumLength      resw 1  ; USHORT
    .Buffer             resd 1  ; WCHAR*
endstruc
ASSERT_STRUCT_SIZE ANSI_STRING, 8

struc OBJECT_ATTRIBUTES
    .RootDirectory      resd 1  ; HANDLE
    .ObjectName         resd 1  ; ANSI_STRING*
    .Attributes         resd 1  ; ULONG
endstruc
ASSERT_STRUCT_SIZE OBJECT_ATTRIBUTES, 12

struc IO_STATUS_BLOCK
    .Status             resd 1  ; LONG
    .Information        resd 1  ; ULONG_PTR
endstruc
ASSERT_STRUCT_SIZE IO_STATUS_BLOCK, 8


; File share:
%define FILE_SHARE_READ                 00000001h
%define FILE_SHARE_WRITE                00000002h

; Access rights:
%define GENERIC_READ                    80000000h
%define GENERIC_WRITE                   40000000h
%define SYNCHRONIZE                     00100000h


; Status codes:
%define STATUS_SUCCESS                  00000000h
%define STATUS_PENDING                  00000103h

;------------------------------------------------------------------------------
; ATA definitions:

; Register addresses:
%define ATA_COMMAND_PORT                01F0h

%define ATA_ERROR_REGISTER              ATA_COMMAND_PORT + 1
%define ATA_SECTOR_COUNT_REGISTER       ATA_COMMAND_PORT + 2
%define ATA_DEVICE_SELECT_REGISTER      ATA_COMMAND_PORT + 6
%define ATA_COMMAND_REGISTER            ATA_COMMAND_PORT + 7
%define ATA_STATUS_REGISTER             ATA_COMMAND_PORT + 7

; ATA commands:
%define ATA_CMD_IDENTIFY                0ECh

; ATA pass through IOCTL:
%define IOCTL_IDE_PASS_THROUGH          0004D028h

struc IDEREGS
    .bFeaturesReg           resb 1  ; UCHAR
    .bSectorCountReg        resb 1  ; UCHAR
    .bSectorNumberReg       resb 1  ; UCHAR
    .bCylLowReg             resb 1  ; UCHAR
    .bCylHighReg            resb 1  ; UCHAR
    .bDriveHeadReg          resb 1  ; UCHAR
    .bCommandReg            resb 1  ; UCHAR
    .bHostSendsData         resb 1  ; UCHAR
endstruc
ASSERT_STRUCT_SIZE IDEREGS, 8

struc ATA_PASS_THROUGH_EX
    .IdeReg                 resb 8  ; IDEREGS
    .DataBufferSize         resd 1  ; ULONG
    .DataBuffer             resd 1  ; VOID*
endstruc
ASSERT_STRUCT_SIZE ATA_PASS_THROUGH_EX, 16

%endif ; _XBOXKRNL_H