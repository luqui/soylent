#ifndef __UTIL_H__
#define __UTIL_H__

#include <cstdlib>
#include "tweak.h"
#include "vec.h"
#include <SDL.h>
#include <string>

inline float randrange(float min, float max) {
	return float(rand()) / RAND_MAX * (max - min) + min;
}

inline vec random_boundary_point() {
	if (rand() % 2) {
		return vec(randrange(CLAMPW, W-CLAMPW-1), rand() % 2 ? CLAMPH : H-CLAMPH-1);
	}
	else {
		return vec(rand() % 2 ? CLAMPW : W-CLAMPW-1, randrange(CLAMPH, H-CLAMPH-1));
	}
}

class FluidGrid;
void add_wide_velocity(FluidGrid* field, vec pos, vec vel);

inline void draw_string(std::string s) {
	for (int i = 0; i < int(s.size()); i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, s[i]);
	}
}

inline void show_error_screen(std::string error) {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1,0.4,0.4);
	glRasterPos2f(W/3, H/2);
	draw_string(error);
	SDL_GL_SwapBuffers();
	
	SDL_Event e;
	while (SDL_WaitEvent(&e)) {
		if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN 
				|| e.type == SDL_JOYBUTTONDOWN) {
			break;
		}
	}
}

inline bool is_a_number(float x) {
	return (x > -HUGE_VAL && x < HUGE_VAL);
}

#endif
