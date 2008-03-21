#ifndef __DRAWING_H__
#define __DRAWING_H__

#include "tweak.h"
#include "vec.h"

class GPUFluid;

void scale_color(float density, float alpha = 1);

void set_color_at(const GPUFluid* field, int x, int y);

void draw_rect(vec ll, vec ur);

void init_color();

void draw_board(const GPUFluid* field, int w, int h);

#endif
