#pragma once

#include <mkb.h>

namespace draw
{

static constexpr s32 DEBUG_CHAR_WIDTH = 0xc;

enum class Color
{
    White,
    Red,
    Orange,
    Blue,
    Pink,
    Purple,
    Green,
};

// Call once during mod initialization
void init();

// Call once per frame in the mkb 2d drawing hook
void disp();

// Call once per frame in the mkb 2d drawing hook before all other disp functions of other things
void predraw();

/*
 * Functions which draw immediately
 */

void rect(float x1, float y1, float x2, float y2, mkb::GXColor color);
void debug_text_palette();
void debug_text(s32 x, s32 y, mkb::GXColor color, char *format, ...);
void debug_text(s32 x, s32 y, Color color, char *format, ...);

/*
 * Functions which cause drawing during disp() and don't necessarily need to be called each frame
 */

// Show a notification in the bottom-right of the screen which fades out after a short period
void notify(Color color, char *format, ...);

}
