
#pragma once
#include <stdlib.h>

// We define WINBASEAPI here to prevent it from getting defined as __declspec(dllimport) which causes
// win32 and standard library functions to get marked as dll imports. If we don't do this the compiler
// will treat them as imports and expect to do a call [ds:XXXXXXXX] instead of calling them directly.
#define WINBASEAPI
#include <Windows.h>
#include "Xbox/Xbox.h"

// Macro to indicate a function uses a custom calling convention (created during LTCG):
#define __usercall

// Macro to wrap inlined uses of version specific game addresses (makes searching for uses of hard coded addresses easy):
#define VERSION_SPECIFIC_ADDR(a)        a

//// Basic type definitions:
//typedef char                CHAR;
//typedef unsigned char       UCHAR;
//typedef short               SHORT;
//typedef unsigned short      USHORT, WORD;
//typedef int                 LONG;
//typedef unsigned int        ULONG, DWORD;
//
//typedef int                 BOOL;
//
//typedef void*               HANDLE;