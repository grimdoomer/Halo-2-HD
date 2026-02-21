
#pragma once
#include "Halo2HD.h"

// Basic type definitions:
typedef unsigned char byte;
typedef unsigned short word;

typedef unsigned int string_id;

typedef unsigned int group_tag;

typedef unsigned int datum_index;

#define DATUM_NONE  0xffffffff

struct rgb_color
{
    char pad;
    char r;
    char g;
    char b;
};

struct argb_color
{
    char a;
    char r;
    char g;
    char b;
};

struct real_rgb_color
{
    float r;
    float g;
    float b;
};

struct real_argb_color
{
    float a;
    float r;
    float g;
    float b;
};

struct real_hsv_color
{
    float hue;
    float saturation;
    float value;
};

struct real_ahsv_color
{
    float alpha;
    float hue;
    float saturation;
    float value;
};


struct short_bounds
{
    word lower;
    word upper;
};

struct angle_bounds
{
    float lower;
    float upper;
};

struct real_bounds
{
    float lower;
    float upper;
};

struct real_fraction_bounds
{
    float lower;
    float upprt;
};


struct point_2d
{
    word x;
    word y;
};

struct rectangle_2d
{
    word y0;
    word x0;
    word y1;
    word x1;
};

struct real_point_2d
{
    float x;
    float y;
};

struct real_point_3d
{
    float x;
    float y;
    float z;
};

struct real_vector_2d
{
    float x;
    float y;
};

struct real_vector_3d
{
    float x;
    float y;
    float z;
};

struct real_vector_4d
{
    float x;
    float y;
    float z;
    float w;
};

struct real_quaternion
{
    float x;
    float y;
    float z;
    float w;
};

struct real_euler_angles_2d
{
    float yaw;
    float pitch;
};

struct real_euler_angles_3d
{
    float yaw;
    float pitch;
    float roll;
};

struct real_plane_2d
{
    float i;
    float j;
    float d;
};

struct real_plane_3d
{
    float i;
    float j;
    float k;
    float d;
};

struct real_rectangle_2d
{
    float y0;
    float x0;
    float y1;
    float x1;
};

extern "C"
{
    void __cdecl game_main(int, int, int);

    void XapiBootToDash(int, int, int);

    void* XPhysicalAlloc(size_t dwSize, ULONG_PTR ulPhysicalAddress, ULONG_PTR ulAlignment, DWORD flProtect);
    void XPhysicalProtect(void* lpAddress, size_t dwSize, DWORD flNewProtect);

    //void* VirtualAlloc(void* lpAddress, size_t dwSize, DWORD flAllocationType, DWORD flProtect);

    int __cdecl snwprintf(wchar_t* buffer, size_t count, const wchar_t* format, ...);
};