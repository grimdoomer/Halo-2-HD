
#pragma once
#include "Halo2HD.h"

/*
 *  DirectDraw error codes
 */

#define _FACD3D  0x876
#define MAKE_D3DHRESULT( code )  MAKE_HRESULT( 1, _FACD3D, code )

 /*
  * Direct3D Errors
  */

#define D3D_OK                              S_OK

#define D3DERR_WRONGTEXTUREFORMAT               MAKE_D3DHRESULT(2072)
#define D3DERR_UNSUPPORTEDCOLOROPERATION        MAKE_D3DHRESULT(2073)
#define D3DERR_UNSUPPORTEDCOLORARG              MAKE_D3DHRESULT(2074)
#define D3DERR_UNSUPPORTEDALPHAOPERATION        MAKE_D3DHRESULT(2075)
#define D3DERR_UNSUPPORTEDALPHAARG              MAKE_D3DHRESULT(2076)
#define D3DERR_TOOMANYOPERATIONS                MAKE_D3DHRESULT(2077)
#define D3DERR_CONFLICTINGTEXTUREFILTER         MAKE_D3DHRESULT(2078)
#define D3DERR_UNSUPPORTEDFACTORVALUE           MAKE_D3DHRESULT(2079)
#define D3DERR_CONFLICTINGRENDERSTATE           MAKE_D3DHRESULT(2081)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER         MAKE_D3DHRESULT(2082)
#define D3DERR_CONFLICTINGTEXTUREPALETTE        MAKE_D3DHRESULT(2086)
#define D3DERR_DRIVERINTERNALERROR              MAKE_D3DHRESULT(2087)
#define D3DERR_TESTINCOMPLETE                   MAKE_D3DHRESULT(2088) // Xbox extension
#define D3DERR_BUFFERTOOSMALL                   MAKE_D3DHRESULT(2089) // Xbox extension
#define D3DERR_TIMEEXPIRED                      MAKE_D3DHRESULT(2090) // Xbox extension

#define D3DERR_NOTFOUND                         MAKE_D3DHRESULT(2150)
#define D3DERR_MOREDATA                         MAKE_D3DHRESULT(2151)
#define D3DERR_DEVICELOST                       MAKE_D3DHRESULT(2152)
#define D3DERR_DEVICENOTRESET                   MAKE_D3DHRESULT(2153)
#define D3DERR_NOTAVAILABLE                     MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY                 MAKE_D3DHRESULT(380)
#define D3DERR_INVALIDDEVICE                    MAKE_D3DHRESULT(2155)
#define D3DERR_INVALIDCALL                      MAKE_D3DHRESULT(2156)


struct D3DDevice;

/*
 * D3DResource, IDirect3DResource8 interface
 *
 * The root structure of all D3D 'resources' such as textures and vertex buffers.
 */

#define D3DCOMMON_REFCOUNT_MASK      0x0000FFFF

#define D3DCOMMON_TYPE_MASK          0x00070000
#define D3DCOMMON_TYPE_SHIFT         16
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define D3DCOMMON_TYPE_PUSHBUFFER    0x00020000
#define D3DCOMMON_TYPE_PALETTE       0x00030000
#define D3DCOMMON_TYPE_TEXTURE       0x00040000
#define D3DCOMMON_TYPE_SURFACE       0x00050000
#define D3DCOMMON_TYPE_FIXUP         0x00060000

#define D3DCOMMON_INTREFCOUNT_MASK   0x00780000
#define D3DCOMMON_INTREFCOUNT_SHIFT  19

 // This flag was used for UMA emulation on pre-Beta development kit
 // machines, and is deprecated on the final hardware.
 //
#define D3DCOMMON_VIDEOMEMORY        0

// Internal flag to indicate that this resource was created by Direct3D
//
#define D3DCOMMON_D3DCREATED         0x01000000

// The rest of the bits may be used by derived classes.
#define D3DCOMMON_UNUSED_MASK        0xFE000000
#define D3DCOMMON_UNUSED_SHIFT       25

struct D3DResource
{
    /* 0x00 */ DWORD Common;           // Refcount and flags common to all resources
    /* 0x04 */ DWORD Data;             // Offset to the data held by this resource
    /* 0x08 */ DWORD Lock;             // Lock information, initialize to zero

    ULONG AddRef();
    ULONG Release();
};
static_assert(sizeof(D3DResource) == 0xC, "D3DResource incorrect struct size");


/*
 * D3DPixelContainer interface
 *
 * A base structure that describes the shared layout between textures
 * and surfaces.
 */

 // The layout of the Format field.

#define D3DFORMAT_RESERVED1_MASK        0x00000003      // Must be zero

#define D3DFORMAT_DMACHANNEL_MASK       0x00000003
#define D3DFORMAT_DMACHANNEL_A          0x00000001      // DMA channel A - the default for all system memory
#define D3DFORMAT_DMACHANNEL_B          0x00000002      // DMA channel B - unused
#define D3DFORMAT_CUBEMAP               0x00000004      // Set if the texture if a cube map
#define D3DFORMAT_BORDERSOURCE_COLOR    0x00000008
#define D3DFORMAT_DIMENSION_MASK        0x000000F0      // # of dimensions
#define D3DFORMAT_DIMENSION_SHIFT       4
#define D3DFORMAT_FORMAT_MASK           0x0000FF00
#define D3DFORMAT_FORMAT_SHIFT          8
#define D3DFORMAT_MIPMAP_MASK           0x000F0000
#define D3DFORMAT_MIPMAP_SHIFT          16
#define D3DFORMAT_USIZE_MASK            0x00F00000      // Log 2 of the U size of the base texture
#define D3DFORMAT_USIZE_SHIFT           20
#define D3DFORMAT_VSIZE_MASK            0x0F000000      // Log 2 of the V size of the base texture
#define D3DFORMAT_VSIZE_SHIFT           24
#define D3DFORMAT_PSIZE_MASK            0xF0000000      // Log 2 of the P size of the base texture
#define D3DFORMAT_PSIZE_SHIFT           28

// The layout of the size field, used for non swizzled or compressed textures.
//
// The Size field of a container will be zero if the texture is swizzled or compressed.
// It is guarenteed to be non-zero otherwise because either the height/width will be
// greater than one or the pitch adjust will be nonzero because the minimum texture
// pitch is 8 bytes.

#define D3DSIZE_WIDTH_MASK              0x00000FFF   // Width of the texture - 1, in texels
#define D3DSIZE_HEIGHT_MASK             0x00FFF000   // Height of the texture - 1, in texels
#define D3DSIZE_HEIGHT_SHIFT            12
#define D3DSIZE_PITCH_MASK              0xFF000000   // Pitch / 64 - 1
#define D3DSIZE_PITCH_SHIFT             24

#define D3DTEXTURE_ALIGNMENT            128
#define D3DTEXTURE_CUBEFACE_ALIGNMENT   128

#define D3DTEXTURE_PITCH_ALIGNMENT 64
#define D3DTEXTURE_PITCH_MIN       64

struct D3DPixelContainer : public D3DResource
{
    /* 0x0C */ DWORD Format;   // Format information about the texture.
    /* 0x10 */ DWORD Size;     // Size of a non power-of-2 texture, must be zero otherwise
};
static_assert(sizeof(D3DPixelContainer) == 0x14, "D3DPixelContainer incorrect struct size");


/*
 * D3DBaseTexture interface
 *
 * The root structure of all D3D textures.  Inherits all of the methods
 * from D3DResource.
 *
 * The data memory pointed to by the Data field must be aligned on a
 * D3DTEXTURE_ALIGNMENT byte multiple.
 */

struct D3DBaseTexture : public D3DPixelContainer
{

};
static_assert(sizeof(D3DBaseTexture) == 0x14, "D3DBaseTexture incorrect struct size");


/*
 * D3DTexture, IDirect3DTexture8 interface
 *
 * A normal texture.  Inherits from D3DBaseTexture
 */

struct D3DTexture : public D3DBaseTexture
{

};
static_assert(sizeof(D3DTexture) == 0x14, "D3DTexture incorrect struct size");


/*
 * D3DSurface, IDirect3DSurface8 interface
 *
 * Abstracts a chunk of data that can be drawn to.  The Common and Format
 * fields use the D3DCOMMON and D3DFORMAT constants defined for
 * textures.
 */

#define D3DSURFACE_ALIGNMENT    D3D_RENDER_MEMORY_ALIGNMENT
#define D3DSURFACE_OWNSMEMORY   0x80000000

struct D3DSurface : public D3DPixelContainer
{
    /* 0x14 */ D3DBaseTexture* Parent;
};
static_assert(sizeof(D3DSurface) == 0x18, "D3DSurface incorrect struct size");


/* Direct3D8 Device types */
typedef enum _D3DDEVTYPE
{
    D3DDEVTYPE_HAL = 1,
    D3DDEVTYPE_REF = 2,
    D3DDEVTYPE_SW = 3,

    D3DDEVTYPE_FORCE_DWORD = 0x7fffffff
} D3DDEVTYPE;

/* AntiAliasing buffer types */

typedef DWORD D3DMULTISAMPLE_TYPE;

#define D3DMULTISAMPLE_NONE                                      0x0011

// Number of samples, sample type, and filter (Xbox extensions):
//
#define D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR              0x1021
#define D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX            0x1121
#define D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR   0x2021
#define D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR     0x2012

#define D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR              0x1022
#define D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN            0x1222
#define D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR              0x2022
#define D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN            0x2222

#define D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN            0x1233
#define D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN            0x2233

// Format of the pre-filter (big) color buffer (Xbox extensions):
//
#define D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT                  0x00000
#define D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5                 0x10000
#define D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5                   0x20000
#define D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8                 0x30000
#define D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8                 0x40000

// Defaults:
//
#define D3DMULTISAMPLE_2_SAMPLES D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX
#define D3DMULTISAMPLE_4_SAMPLES D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN
#define D3DMULTISAMPLE_9_SAMPLES D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN

/* Types for D3DRS_MULTISAMPLEMODE and D3DRS_MULTISAMPLERENDERTARGETMODE */
typedef enum _D3DMULTISAMPLEMODE
{
    D3DMULTISAMPLEMODE_1X = 0,
    D3DMULTISAMPLEMODE_2X = 1,
    D3DMULTISAMPLEMODE_4X = 2,

    D3DMULTISAMPLEMODE_FORCE_DWORD = 0x7fffffff
} D3DMULTISAMPLEMODE;


/* Formats
 * Most of these names have the following convention:
 *      A = Alpha
 *      R = Red
 *      G = Green
 *      B = Blue
 *      X = Unused Bits
 *      P = Palette
 *      L = Luminance
 *      U = dU coordinate for BumpMap
 *      V = dV coordinate for BumpMap
 *      S = Stencil
 *      D = Depth (e.g. Z or W buffer)
 *
 *      Further, the order of the pieces are from MSB first; hence
 *      D3DFMT_A8L8 indicates that the high byte of this two byte
 *      format is alpha.
 *
 *      D16 indicates:
 *           - An integer 16-bit value.
 *           - An app-lockable surface.
 *
 *      All Depth/Stencil formats except D3DFMT_D16_LOCKABLE indicate:
 *          - no particular bit ordering per pixel, and
 *          - are not app lockable, and
 *          - the driver is allowed to consume more than the indicated
 *            number of bits per Depth channel (but not Stencil channel).
 */

typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN = 0xFFFFFFFF,

    /* Swizzled formats */

    D3DFMT_A8R8G8B8 = 0x00000006,
    D3DFMT_X8R8G8B8 = 0x00000007,
    D3DFMT_R5G6B5 = 0x00000005,
    D3DFMT_R6G5B5 = 0x00000027,
    D3DFMT_X1R5G5B5 = 0x00000003,
    D3DFMT_A1R5G5B5 = 0x00000002,
    D3DFMT_A4R4G4B4 = 0x00000004,
    D3DFMT_A8 = 0x00000019,
    D3DFMT_A8B8G8R8 = 0x0000003A,
    D3DFMT_B8G8R8A8 = 0x0000003B,
    D3DFMT_R4G4B4A4 = 0x00000039,
    D3DFMT_R5G5B5A1 = 0x00000038,
    D3DFMT_R8G8B8A8 = 0x0000003C,
    D3DFMT_R8B8 = 0x00000029,
    D3DFMT_G8B8 = 0x00000028,

    D3DFMT_P8 = 0x0000000B,

    D3DFMT_L8 = 0x00000000,
    D3DFMT_A8L8 = 0x0000001A,
    D3DFMT_AL8 = 0x00000001,
    D3DFMT_L16 = 0x00000032,

    D3DFMT_V8U8 = 0x00000028,
    D3DFMT_L6V5U5 = 0x00000027,
    D3DFMT_X8L8V8U8 = 0x00000007,
    D3DFMT_Q8W8V8U8 = 0x0000003A,
    D3DFMT_V16U16 = 0x00000033,

    D3DFMT_D16_LOCKABLE = 0x0000002C,
    D3DFMT_D16 = 0x0000002C,
    D3DFMT_D24S8 = 0x0000002A,
    D3DFMT_F16 = 0x0000002D,
    D3DFMT_F24S8 = 0x0000002B,

    /* YUV formats */

    D3DFMT_YUY2 = 0x00000024,
    D3DFMT_UYVY = 0x00000025,

    /* Compressed formats */

    D3DFMT_DXT1 = 0x0000000C,
    D3DFMT_DXT2 = 0x0000000E,
    D3DFMT_DXT3 = 0x0000000E,
    D3DFMT_DXT4 = 0x0000000F,
    D3DFMT_DXT5 = 0x0000000F,

    /* Linear formats */

    D3DFMT_LIN_A1R5G5B5 = 0x00000010,
    D3DFMT_LIN_A4R4G4B4 = 0x0000001D,
    D3DFMT_LIN_A8 = 0x0000001F,
    D3DFMT_LIN_A8B8G8R8 = 0x0000003F,
    D3DFMT_LIN_A8R8G8B8 = 0x00000012,
    D3DFMT_LIN_B8G8R8A8 = 0x00000040,
    D3DFMT_LIN_G8B8 = 0x00000017,
    D3DFMT_LIN_R4G4B4A4 = 0x0000003E,
    D3DFMT_LIN_R5G5B5A1 = 0x0000003D,
    D3DFMT_LIN_R5G6B5 = 0x00000011,
    D3DFMT_LIN_R6G5B5 = 0x00000037,
    D3DFMT_LIN_R8B8 = 0x00000016,
    D3DFMT_LIN_R8G8B8A8 = 0x00000041,
    D3DFMT_LIN_X1R5G5B5 = 0x0000001C,
    D3DFMT_LIN_X8R8G8B8 = 0x0000001E,

    D3DFMT_LIN_A8L8 = 0x00000020,
    D3DFMT_LIN_AL8 = 0x0000001B,
    D3DFMT_LIN_L16 = 0x00000035,
    D3DFMT_LIN_L8 = 0x00000013,

    D3DFMT_LIN_V16U16 = 0x00000036,
    D3DFMT_LIN_V8U8 = 0x00000017,
    D3DFMT_LIN_L6V5U5 = 0x00000037,
    D3DFMT_LIN_X8L8V8U8 = 0x0000001E,
    D3DFMT_LIN_Q8W8V8U8 = 0x00000012,

    D3DFMT_LIN_D24S8 = 0x0000002E,
    D3DFMT_LIN_F24S8 = 0x0000002F,
    D3DFMT_LIN_D16 = 0x00000030,
    D3DFMT_LIN_F16 = 0x00000031,

    D3DFMT_VERTEXDATA = 100,
    D3DFMT_INDEX16 = 101,

    D3DFMT_FORCE_DWORD = 0x7fffffff
} D3DFORMAT;

/* Display mode flags */

#define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER      0x00000001
#define D3DPRESENTFLAG_WIDESCREEN               0x00000010
#define D3DPRESENTFLAG_INTERLACED               0x00000020
#define D3DPRESENTFLAG_PROGRESSIVE              0x00000040
#define D3DPRESENTFLAG_FIELD                    0x00000080
#define D3DPRESENTFLAG_10X11PIXELASPECTRATIO    0x00000100
#define D3DPRESENTFLAG_EMULATE_REFRESH_RATE     0x00000200

/* SwapEffects */
typedef enum _D3DSWAPEFFECT
{
    D3DSWAPEFFECT_DISCARD = 1,
    D3DSWAPEFFECT_FLIP = 2,
    D3DSWAPEFFECT_COPY = 3,
    D3DSWAPEFFECT_COPY_VSYNC = 4,

    D3DSWAPEFFECT_FORCE_DWORD = 0x7fffffff
} D3DSWAPEFFECT;

/* Swap flags (Xbox extension) */
#define D3DSWAP_COPY                0x00000001L
#define D3DSWAP_BYPASSCOPY          0x00000002L
#define D3DSWAP_FINISH              0x00000004L

#define D3DSWAP_DEFAULT             0x00000000L

//
// PresentationIntervals
//
#define D3DPRESENT_INTERVAL_DEFAULT             0x00000000L
#define D3DPRESENT_INTERVAL_ONE                 0x00000001L
#define D3DPRESENT_INTERVAL_TWO                 0x00000002L
#define D3DPRESENT_INTERVAL_THREE               0x00000004L
#define D3DPRESENT_INTERVAL_FOUR                0x00000008L
#define D3DPRESENT_INTERVAL_IMMEDIATE           0x80000000L
#define D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE    0x80000001L
#define D3DPRESENT_INTERVAL_TWO_OR_IMMEDIATE    0x80000002L
#define D3DPRESENT_INTERVAL_THREE_OR_IMMEDIATE  0x80000004L

/* Reset and CreateDevice Parameters */
typedef struct _D3DPRESENT_PARAMETERS_
{
    UINT                BackBufferWidth;
    UINT                BackBufferHeight;
    D3DFORMAT           BackBufferFormat;
    UINT                BackBufferCount;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    D3DSWAPEFFECT       SwapEffect;
    HWND                hDeviceWindow;
    BOOL                Windowed;
    BOOL                EnableAutoDepthStencil;
    D3DFORMAT           AutoDepthStencilFormat;
    DWORD               Flags;
    UINT                FullScreen_RefreshRateInHz;
    UINT                FullScreen_PresentationInterval;
    D3DSurface* BufferSurfaces[3];
    D3DSurface* DepthStencilSurface;

} D3DPRESENT_PARAMETERS;

#define D3DCREATE_HARDWARE_VERTEXPROCESSING     0x00000040L


/* Back buffer types */
typedef DWORD D3DBACKBUFFER_TYPE;

#define D3DBACKBUFFER_TYPE_MONO   0

/* Usages */
#define D3DUSAGE_RENDERTARGET           (0x00000001L)
#define D3DUSAGE_DEPTHSTENCIL           (0x00000002L)

/* Usages for Vertex/Index buffers */
#define D3DUSAGE_WRITEONLY              (0x00000008L)
#define D3DUSAGE_POINTS                 (0x00000040L)
#define D3DUSAGE_RTPATCHES              (0x00000080L)
#define D3DUSAGE_DYNAMIC                (0x00000200L)

/* Usages for CreateVertexShader */
#define D3DUSAGE_PERSISTENTDIFFUSE      (0x00000400L)   // Xbox extension
#define D3DUSAGE_PERSISTENTSPECULAR     (0x00000800L)   // Xbox extension
#define D3DUSAGE_PERSISTENTBACKDIFFUSE  (0x00001000L)   // Xbox extension
#define D3DUSAGE_PERSISTENTBACKSPECULAR (0x00002000L)   // Xbox extension

/* Usages for CreateTexture/CreateImageSurface */
#define D3DUSAGE_BORDERSOURCE_COLOR     (0x00000000L)   // Xbox extension
#define D3DUSAGE_BORDERSOURCE_TEXTURE   (0x00010000L)   // Xbox extension


struct D3DDevice
{
    HRESULT GetBackBuffer(int BackBuffer, D3DBACKBUFFER_TYPE Type, D3DSurface** ppBackBuffer);
    HRESULT GetDepthStencilSurface(D3DSurface** ppZStencilSurface);
};
