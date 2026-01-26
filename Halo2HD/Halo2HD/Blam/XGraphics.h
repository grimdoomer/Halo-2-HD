
#pragma once
#include "d3d8types.h"

namespace XGRAPHICS
{
#define FMT_RENDERTARGET        0x80
#define FMT_DEPTHBUFFER         0x40
#define FMT_BITSPERPIXEL        0x3c
#define FMT_LINEAR              0x02
#define FMT_SWIZZLED            0x01

#define FMT_32BPP               0x20
#define FMT_16BPP               0x10
#define FMT_8BPP                0x08
#define FMT_4BPP                0x04

    extern "C"
    {
        extern const BYTE g_TextureFormat[];
    };

    inline bool IsSwizzledD3DFORMAT(D3DFORMAT Format)
    {
        return (g_TextureFormat[Format] & FMT_SWIZZLED) != 0;
    }

    inline bool IsCompressedD3DFORMAT(D3DFORMAT Format)
    {
        switch (Format)
        {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
            return true;
        }

        return false;
    }

    inline DWORD BitsPerPixelOfD3DFORMAT(D3DFORMAT Format)
    {
        return g_TextureFormat[Format] & FMT_BITSPERPIXEL;
    }

    inline DWORD __fastcall Log2(DWORD Value)
    {
        __asm { bsf     eax, [Value] }
    }

    inline DWORD MinimumTextureSizeOfD3DFormat(D3DFORMAT Format)
    {
        return IsCompressedD3DFORMAT(Format) ? 2 : 0;
    }

    inline DWORD CalcPitch(DWORD Width, DWORD TexelSize)
    {
        return (Width * TexelSize / 8 + D3DTEXTURE_PITCH_ALIGNMENT - 1) & ~(D3DTEXTURE_PITCH_ALIGNMENT - 1);
    }

    DWORD EncodeFormat(DWORD Width, DWORD Height, DWORD Depth, DWORD Levels, D3DFORMAT Format, DWORD Pitch, bool isCubeMap, bool isVolume, DWORD* pFormat, DWORD* pSize)
    {
        DWORD Size = 0;
        DWORD TexelSize = BitsPerPixelOfD3DFORMAT(Format);
        DWORD LogWidth = 0;
        DWORD LogHeight = 0;
        DWORD LogDepth = 0;
        DWORD SizeWidth = 0;
        DWORD SizeHeight = 0;

        // Swizzled and compressed textures are always power of 2, linear textures are not required to have power of 2 dimensions.
        if (IsSwizzledD3DFORMAT(Format) == true || IsCompressedD3DFORMAT(Format) == true)
        {
            // Calculate the log dimensions of the texture.
            LogWidth = Log2(Width);
            LogHeight = Log2(Height);
            LogDepth = Log2(Depth);

            DWORD LogMin = MinimumTextureSizeOfD3DFormat(Format);

            if (Levels == 0)
                Levels = max(LogWidth, max(LogHeight, LogDepth)) + 1;

            // Calculate the total size of the texture.
            DWORD CurrentWidth = LogWidth;
            DWORD CurrentHeight = LogHeight;
            DWORD CurrentDepth = LogDepth;

            for (int currentLevel = Levels; currentLevel > 0; currentLevel--)
            {
                DWORD logSize = max(CurrentWidth, LogMin) + max(CurrentHeight, LogMin) + CurrentDepth;

                Size += (1 << logSize) * TexelSize / 8;

                if (CurrentWidth > 0)
                    CurrentWidth--;

                if (CurrentHeight > 0)
                    CurrentHeight--;

                if (CurrentDepth > 0)
                    CurrentDepth--;
            }

            // Is the texture is a cubemap round the face size up to the nearest alignment interval and multiply by the cube face count.
            if (isCubeMap == true)
                Size = ((Size + D3DTEXTURE_CUBEFACE_ALIGNMENT - 1) & ~(D3DTEXTURE_CUBEFACE_ALIGNMENT - 1)) * 6;
        }
        else
        {
            if (Levels == 0)
                Levels = 1;

            if (Pitch == 0)
                Pitch = CalcPitch(Width, TexelSize);

            SizeWidth = Width;
            SizeHeight = Height;

            Size = Pitch * Height;
        }

        // Build the format value.
        *pFormat = (isCubeMap == true ? D3DFORMAT_CUBEMAP : 0) |
            (isVolume == true ? 3 : 2) << D3DFORMAT_DIMENSION_SHIFT |
            Format << D3DFORMAT_FORMAT_SHIFT |
            Levels << D3DFORMAT_MIPMAP_SHIFT |
            LogWidth << D3DFORMAT_USIZE_SHIFT |
            LogHeight << D3DFORMAT_VSIZE_SHIFT |
            LogDepth << D3DFORMAT_PSIZE_SHIFT |
            D3DFORMAT_DMACHANNEL_A |
            D3DFORMAT_BORDERSOURCE_COLOR;

        if (SizeWidth != 0)
        {
            *pSize = (SizeWidth - 1) |
                (SizeHeight - 1) << D3DSIZE_HEIGHT_SHIFT |
                ((Pitch / D3DTEXTURE_PITCH_ALIGNMENT) - 1) << D3DSIZE_PITCH_SHIFT;
        }
        else
        {
            *pSize = 0;
        }

        return Size;
    }

    void EncodeTexture(DWORD Width, DWORD Height, DWORD Depth, DWORD Levels, DWORD Usage, D3DFORMAT Format, DWORD Pitch, bool isCubeMap, bool isVolume, DWORD Data, D3DTexture* pTexture)
    {
        EncodeFormat(Width, Height, Depth, Levels, Format, Pitch, isCubeMap, isVolume, &pTexture->Format, &pTexture->Size);

        if ((Usage & D3DUSAGE_BORDERSOURCE_TEXTURE) != 0)
            pTexture->Format &= ~D3DFORMAT_BORDERSOURCE_COLOR;

        pTexture->Common = 1 | D3DCOMMON_TYPE_TEXTURE | D3DCOMMON_VIDEOMEMORY;
        pTexture->Lock = 0;
        pTexture->Data = Data;
    }

    void WINAPI XGSetTextureHeader(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, /* D3DPOOL */ UINT Pool, D3DTexture* pTexture, UINT Data, UINT Pitch)
    {
        EncodeTexture(Width, Height, 1, Levels, Usage, Format, Pitch, false, false, Data, pTexture);
    }
}