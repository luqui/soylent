#ifndef __SOYLIB_DRAWING_H__
#define __SOYLIB_DRAWING_H__

#include "soy/config.h"
#include "soy/vec2.h"

void DLLEXPORT draw_square(vec2 ll, vec2 ur);

// Draws a square with the currently bound texture mapped at its four
// corners.  Assumes a positive-y orientation.
void DLLEXPORT draw_textured_square(vec2 ll, vec2 ur);

#endif
