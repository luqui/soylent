#include "config.h"

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <stack>

using std::vector;

#include <soy/vec2.h>
#include <soy/Timer.h>
#include "Universe.h"
#include "GenerationalRule.h"
#include "TwoPlayerStarWarsRule.h"
#include "Utils.h"
#include "ViewBox.h"
#include "MainGame.h"


int XRES = 1280;
int YRES = 1024;
float ASPECT_RATIO = float(YRES)/XRES;
int zoomWidth = 64;
int zoomHeight = int(ASPECT_RATIO * zoomWidth);
double zoomStep = 1.2;
int W = 320;
int H = int(ASPECT_RATIO * W);

float DT = 0.05;

double ALPHA_FACTOR = 1;

bool QUIT = true;
bool PAUSE = true;
bool ZOOM = false;

typedef TwoPlayerStarWarsRule Rule;
typedef MainGame game_t;
game_t* GAME = 0;
ViewBox* WORLDVIEW = 0;
std::stack<ViewBox> VIEWSTACK;
Timer TIMER;

void step()
{
    static int frames = 0;
    static float timer = 0;
    
    static float drawtime = 0;
    DT = TIMER.get_time_diff();
    
    GAME->step();

    drawtime -= DT;
    if (drawtime <= 0) {
        if(!PAUSE) GAME->integrate();
        drawtime = FRAMETIME;
    }

    frames++;
    timer += DT;
    if (timer > 1) {
        std::cout << frames << " FHz\n";
        frames = 0;
        timer = 0;
    }
}

void set_resolution()
{
	SDL_Rect** modes;
	SDL_Surface* screen;

	modes = SDL_ListModes(NULL, SDL_OPENGL | (FULLSCREEN ? SDL_FULLSCREEN : 0));

	int numModes;
	int x = 0, y = 0;
	
	if ((int)modes != -1) {
		for (numModes = 0; modes[numModes]; numModes++) {
			if(modes[numModes]->w * modes[numModes]->h > x * y) {
				x = modes[numModes]->w;
				y = modes[numModes]->h;
			}
		}
	}
	else {
		x = 640;
		y = 480;
	}

	screen = SDL_SetVideoMode(x, y, 0, SDL_OPENGL | (FULLSCREEN ? SDL_FULLSCREEN : 0));
	XRES = x;
	YRES = y;

    std::cout << XRES << "x" << YRES << "\n";

	ASPECT_RATIO = float(YRES)/XRES;
	zoomHeight = int(ASPECT_RATIO * zoomWidth);
	H = int(ASPECT_RATIO * W);
		
	glViewport(0,0,XRES,YRES);

    GAME = new game_t(W,H);
}

void init_sdl() 
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	set_resolution();
	
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, W, 0, H);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	srand(time(NULL));

    glEnable(GL_TEXTURE_2D);
}


void events() 
{
	SDL_Event e;

	int x, y;
	SDL_GetMouseState(&x, &y);
	vec2 mouse = VIEWSTACK.top().mouse_to_world(vec2(x, y));

	while (SDL_PollEvent(&e)) {
		GAME->events(&e);
		if (e.type == SDL_KEYDOWN) {
			if(e.key.keysym.sym == SDLK_ESCAPE) {
				QUIT = true;
			}
			if(e.key.keysym.sym == SDLK_SPACE) {
				PAUSE = !PAUSE;
			}
			if(e.key.keysym.sym == SDLK_z) {
				ViewBox curbox = VIEWSTACK.top();
				if (curbox.width <= zoomWidth) {
					change_view(*WORLDVIEW);
				}
				else {
					change_view(new_box(curbox, vec2(zoomWidth, zoomHeight), mouse));
				}
			}
			if(e.key.keysym.sym == SDLK_s) {
				GAME->integrate();
				PAUSE = true;
			}
			if(e.key.keysym.sym == SDLK_BACKSPACE) {
				GAME->clear();
				GAME->reset();
				PAUSE = true;
			}
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			ViewBox curbox = VIEWSTACK.top();

			if(e.button.button == SDL_BUTTON_WHEELUP) {//zoom in
				if (curbox.width / zoomStep >= zoomWidth) {
					change_view(new_box(curbox, vec2(curbox.width, curbox.height) / zoomStep, mouse));
				}
				else {
					change_view(new_box(curbox, vec2(zoomWidth, zoomHeight), mouse));
				}

			}
			else if (e.button.button == SDL_BUTTON_WHEELDOWN) {//zoom out
				if (curbox.width * zoomStep <= W) {
					change_view(new_box(curbox, vec2(curbox.width, curbox.height) * zoomStep, mouse));
				}
				else {
					change_view(ViewBox(W, H, 0, 0));
				}
			}
		}
	}
}

void draw()
{
	GAME->draw();
}

void reset()
{
	while(!VIEWSTACK.empty()) {
		VIEWSTACK.pop();
	}

	WORLDVIEW = new ViewBox(W,H,0,0);
	VIEWSTACK.push(*WORLDVIEW);
}

void run()
{
	reset();
	QUIT = false;
	while (!QUIT) {
		events();
		step();
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		draw();
		SDL_GL_SwapBuffers();
	}
}

int main(int argc, char** argv) 
{
	init_sdl();
    TIMER.init();
	run();
	SDL_Quit();
	return 0;
}
