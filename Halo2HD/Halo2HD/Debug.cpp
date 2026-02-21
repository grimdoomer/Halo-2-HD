
#include "Halo2HD.h"
#include "Blam/main/main_time.h"
#include "Blam/rasterizer/xbox/rasterizer_xbox_main.h"
#include "Blam/rasterizer/rasterizer_text.h"
#include "Blam/text/draw_string.h"

float Dbg_CalculateFPS()
{
    // Calculate the starting index.
    int swapArrayIndex = (_swap_count_array_index + 14) % 15;

    float fSwapCounter = 0.0f;
    float fVBlankCounter = 0.0f;

    // Loop and calculate the total number of swap calls and vblank intervals.
    while (swapArrayIndex != _swap_count_array_index)
    {
        fSwapCounter += (float)_swap_count_array[swapArrayIndex];
        fVBlankCounter += 1.0f;
        
        swapArrayIndex = (swapArrayIndex + 14) % 15;
    }

    // Make sure the vblank counter isn't zero.
    if (fVBlankCounter == 0.0f)
        return 0.0f;

    // Calculate the average number of swaps per vblank.
    float fAverageSwapsPerVBlank = fSwapCounter / fVBlankCounter;
    if (fAverageSwapsPerVBlank <= 0.0f)
        return 0.0f;

    // Get the refresh rate and make sure it's valid.
    float fRefreshRate = rasterizer_globals._refresh_rate_hz != 0 ? (float)rasterizer_globals._refresh_rate_hz : 60.0f;

    // Calculate final FPS.
    return fRefreshRate / fAverageSwapsPerVBlank;
}

void Dbg_DrawPerfCounters()
{
    wchar_t sStringBuffer[32];

    // Setup string bounds.
    rectangle_2d bounds =
    {
        100,
        30,
        120,
        200
    };

    // Set text color.
    real_argb_color textColor =
    {
        1.0f,
        0.0f,
        1.0f,
        0.0f
    };
    draw_string_set_color(&textColor);

    // Set text properties.
    draw_string_set_format(_text_style_plain, 0, 0);
    draw_string_set_font((font_id)6);

    // Format and print fps string.
    snwprintf(sStringBuffer, _countof(sStringBuffer), L"FPS: %.1f", Dbg_CalculateFPS());
    rasterizer_draw_string(sStringBuffer, &bounds, nullptr, 0, 0, 1.0f, 0, 0);

    // Update string bounds for the next line.
    bounds.y0 = 120;
    bounds.y1 = 140;

    // Get the current NVPLL_COEFF value.
    volatile ULONG* NVPLL_COEFF = (volatile ULONG*)0xFD680500;
    ULONG coeff = *NVPLL_COEFF;

    /*
        NVPLL_COEFF (32 bits):
	        Bits 0-7: M
	        Bits 8-15: N
	        Bits 16-18: P
	
        BASE_CLK = 16.6667 Mhz
	
        nvclk = (N * BASE_CLK / (1 << P) / M)
    */
    ULONG M = coeff & 0xFF;
    ULONG N = (coeff >> 8) & 0xFF;
    ULONG P = (coeff >> 16) & 7;
    float fGpuClock = (N * 16.666666f / (float)(1 << P) / (float)M);

    // Format and print gpu clock string.
    snwprintf(sStringBuffer, _countof(sStringBuffer), L"GPU Clk: %.2f Mhz", fGpuClock);
    rasterizer_draw_string(sStringBuffer, &bounds, nullptr, 0, 0, 1.0f, 0, 0);
}