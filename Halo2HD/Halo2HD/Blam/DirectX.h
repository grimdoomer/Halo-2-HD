
#pragma once
#include "Halo2HD.h"

// Macro to wrap offsets/values for D3D structures that are version/build specific. Values wrapped with this macro
// may change between different versions of the DirectX library used or build targer (i.e.: debug/release).
#define D3D_VERSION_SPECIFIC(a)         a


#include "d3d8types.h"

extern "C"
{
    // int __usercall Direct3D_CreateDevice@<eax>(int BehaviorFlags@<eax>, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDevice **ppReturnedDeviceInterface@<ecx>);
    HRESULT __usercall WINAPI _Direct3D_CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void* pUnused, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDevice** ppReturnedDeviceInterface);

    // int __usercall D3DDevice_Reset@<eax>(struct D3DPRESENT_PARAMETERS *pPresentationParameters@<ebx>);
    HRESULT WINAPI _D3DDevice_Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);

    // int __usercall IDirect3DDevice8_Swap@<eax>(int flags@<eax>)
    ULONG WINAPI _D3DDevice_Swap(ULONG Flags);

    ULONG WINAPI D3DResource_AddRef(D3DResource* pResource);
    ULONG WINAPI D3DResource_Release(D3DResource* pResource);
};

inline HRESULT __cdecl Direct3D_CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void* pUnused, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDevice** ppReturnedDeviceInterface)
{
    _asm
    {
        push    pPresentationParameters
        mov     ecx, ppReturnedDeviceInterface
        mov     eax, BehaviorFlags
        call    _Direct3D_CreateDevice
    }
}

inline HRESULT __cdecl D3DDevice_Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    _asm
    {
        push    ebx
        mov     ebx, pPresentationParameters
        call    _D3DDevice_Reset
        pop     ebx
    }
}

inline ULONG __cdecl D3DDevice_Swap(ULONG Flags)
{
    _asm
    {
        mov     eax, Flags
        call    _D3DDevice_Swap
    }
}

inline void D3DDevice_GetBackBuffer(D3DDevice* Device, int BackBuffer, D3DBACKBUFFER_TYPE Type, D3DSurface** ppBackBuffer)
{
    // Convert the specific back buffer index to an array index.
    int index;
    if (BackBuffer == -1)
        index = 1;
    else if (BackBuffer == 0)
        index = 0;
    else
        index = 2;

    // Get the back buffer surface.
    *ppBackBuffer = *(D3DSurface**)((BYTE*)Device + D3D_VERSION_SPECIFIC(0x1A14) + (index * 4));
    (*ppBackBuffer)->AddRef();
}

inline HRESULT D3DDevice_GetDepthStencilSurface(D3DDevice* Device, D3DSurface** ppZStencilSurface)
{
    // Get the depth stencil surface.
    *ppZStencilSurface = *(D3DSurface**)((BYTE*)Device + D3D_VERSION_SPECIFIC(0x1A08));
    if (*ppZStencilSurface != nullptr)
    {
        // Inc the ref count.
        (*ppZStencilSurface)->AddRef();
        return S_OK;
    }

    return D3DERR_NOTFOUND;
}

inline ULONG D3DResource::AddRef()
{
    return D3DResource_AddRef(this);
}

inline ULONG D3DResource::Release()
{
    return D3DResource_Release(this);
}

inline HRESULT D3DDevice::GetBackBuffer(int BackBuffer, D3DBACKBUFFER_TYPE Type, D3DSurface** ppBackBuffer)
{
    D3DDevice_GetBackBuffer(this, BackBuffer, Type, ppBackBuffer);
    return S_OK;
}

inline HRESULT D3DDevice::GetDepthStencilSurface(D3DSurface** ppZStencilSurface)
{
    return D3DDevice_GetDepthStencilSurface(this, ppZStencilSurface);
}


namespace D3D
{
    extern "C"
    {
        // int __usercall D3D::CalcTilePitch@<eax>(int width, D3DFORMAT format@<eax>)
        ULONG WINAPI _CalcTilePitch(int width, D3DFORMAT format);
    };

    inline ULONG __cdecl CalcTilePitch(int width, D3DFORMAT format)
    {
        _asm
        {
            mov     eax, format
            push    width
            call    _CalcTilePitch
        }
    }
}