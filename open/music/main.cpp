#include <iostream>

#include <soy/init.h>
#include <soy/Timer.h>
#include <ctime>
#include <GL/gl.h>

#include "GameMode.h"
#include "PlatformMode.h"

SoyInit INIT;

GameMode* MODE = 0;

void init()
{
	//INIT.set_fullscreen();
	INIT.init();
	srand(time(NULL));
}

void events() 
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (MODE->events(&e)) { continue; }

		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE
		 || e.type == SDL_QUIT) {
			exit(0);
		}
	}
}

int main(int argc, char** argv)
{
	init();
	
	FrameRateLockTimer timer(DT);

	MODE = new PlatformMode;

	while (true) {
		events();
		MODE->step();
		
		glClear(GL_COLOR_BUFFER_BIT);
		MODE->draw();
		SDL_GL_SwapBuffers();

		timer.lock();
	}
}
