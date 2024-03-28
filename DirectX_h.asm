
%ifndef _DIRECTX_H
%define _DIRECTX_H

%include "Utilities_h.asm"

; Macro to compile in a D3D function that may not exist in the binary or may have been inlined.
;
; D3D_INCLUDE <function name>
%macro D3D_INCLUDE 1
    %define D3DINC_%1
    HACK_FUNCTION %1
%endmacro


struc D3DPRESENT_PARAMETERS
    .BackBufferWidth                    resd    1
    .BackBufferHeight                   resd    1
    .BackBufferFormat                   resd    1
    .BackBufferCount                    resd    1
    .MultiSampleType                    resd    1
    .SwapEffect                         resd    1
    .hDeviceWindow                      resd    1
    .Windowed                           resd    1
    .EnableAutoDepthStencil             resd    1
    .AutoDepthStencilFormat             resd    1
    .Flags                              resd    1
    .FullScreen_RefreshRateInHz         resd    1
    .FullScreen_PresentationInterval    resd    1
    .BufferSurfaces                     resd    3
    .DepthStencilSurface                resd    1
endstruc
ASSERT_STRUCT_SIZE D3DPRESENT_PARAMETERS, 44h

struc D3DTILE
    .Flags          resd    1
    .pMemory        resd    1
    .Size           resd    1
    .Pitch          resd    1
    .ZStartTag      resd    1
    .ZOffset        resd    1
endstruc
ASSERT_STRUCT_SIZE D3DTILE, 18h

%endif ; _DIRECTX_H