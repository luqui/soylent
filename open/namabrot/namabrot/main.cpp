#include <SDL.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

const int XRES = 1024;
const int YRES = 768;
const int ITERS = 40;

double VIEW_LEFT    = -4;
double VIEW_RIGHT   =  4;
double VIEW_BOTTOM  =  -4;
double VIEW_TOP     =  4;

double cutoff = 0.0;

SDL_Surface* SCREEN;

void init() {
	SDL_Init(SDL_INIT_VIDEO);

	SCREEN = SDL_SetVideoMode(XRES, YRES, 32, 0);
}

bool proc_event(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE || e.type == SDL_QUIT) {
		SDL_Quit();
		exit(0);
		return true;
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			double x = e.button.x;
			double y = e.button.y;
			double x_dist = VIEW_RIGHT - VIEW_LEFT;
			double y_dist = VIEW_TOP - VIEW_BOTTOM;
			double x_center = VIEW_LEFT + x/XRES * x_dist;
			double y_center = VIEW_BOTTOM + y/YRES * y_dist;
			VIEW_LEFT = x_center - x_dist/4;
			VIEW_RIGHT = x_center + x_dist/4;
			VIEW_TOP = y_center + y_dist/4;
			VIEW_BOTTOM = y_center - y_dist/4;
			return true;
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			double x_dist = VIEW_RIGHT - VIEW_LEFT;
			double y_dist = VIEW_TOP - VIEW_BOTTOM;
			double x_center = VIEW_LEFT + (VIEW_RIGHT - VIEW_LEFT) / 2;
			double y_center = VIEW_BOTTOM + (VIEW_TOP - VIEW_BOTTOM) / 2;
			VIEW_LEFT = x_center - x_dist;
			VIEW_RIGHT = x_center + x_dist;
			VIEW_BOTTOM = y_center - y_dist;
			VIEW_TOP = y_center + y_dist;
			return true;
		}
	}
	return false;
}

void poll_events() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		proc_event(e);
	}
}

void wait_events() {
	SDL_Event e;
	while (SDL_WaitEvent(&e)) {
		if (proc_event(e)) { return; }
		if (e.type == SDL_KEYDOWN) { return; }
		
		
	}
}

void plot_point(double zr, double zi, Uint32 color) {
	int xcor = int(XRES * (zr - VIEW_LEFT) / (VIEW_RIGHT - VIEW_LEFT) + 0.5);
	int ycor = int(YRES * (zi - VIEW_BOTTOM) / (VIEW_TOP - VIEW_BOTTOM) + 0.5);

	if (xcor < 0 || xcor >= XRES || ycor < 0 || ycor >= YRES) return;

	Uint32* pixels = (Uint32*)SCREEN->pixels;
	pixels[XRES * ycor + xcor] += color;
}

void compute(double cr, double ci) {
	double zr1 = cr, zi1 = ci;
	double nzr = 0, nzi = 0;

	//double coef = M_PI;
	double coef = 3;
	int i = 0;
	do {
		nzr = cos(coef * cr * atan2(zi1, zr1)) * pow((zi1*zi1 + zr1*zr1), coef * cr/2);
		nzi = sin(coef * ci * atan2(zi1, zr1)) * pow((zi1*zi1 + zr1*zr1), coef * ci/2);

		zr1 = nzr;  zi1 = nzi;
	}
	while (nzr*nzr + nzi*nzi > 0.001 && i++ < ITERS && nzr*nzr + nzi*nzi < cutoff );

	if (i < ITERS) {
		i *= double(i)/ITERS*255;
		if (nzr*nzr + nzi*nzi >= cutoff) {
			plot_point(cr, ci, i);
		}
		else {
			plot_point(cr, ci, (i << 16));
		}
	}
}

void plot() {
	int count = 0;
	double cr;
	double ci;
	SDL_LockSurface(SCREEN);
	for (int x = 0; x < XRES; x++) {
		
		for (int y = 0; y < YRES; y++) {
			cr = (double(x) / double(XRES)) * (VIEW_RIGHT - VIEW_LEFT) + VIEW_LEFT;
			ci = (double(y) / double(YRES)) * (VIEW_TOP - VIEW_BOTTOM) + VIEW_BOTTOM;

			compute(cr, ci);
		}
		
		/*if (count++ % 1 == 0) {
			SDL_UpdateRect(SCREEN, 0, 0, 0, 0);
		}*/
		poll_events();
	}
	SDL_UnlockSurface(SCREEN);
}

int main(int argc, char** argv) {
	init();
	while (true) {
		SDL_FillRect(SCREEN, NULL, 0);
		plot();
		SDL_UpdateRect(SCREEN, 0, 0, 0, 0);
		//wait_events();
		cutoff += 1;
	}
	return 0;
}