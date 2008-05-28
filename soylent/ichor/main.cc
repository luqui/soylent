#include "tweak.h"

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <list>
#include <cmath>
#include <iostream>
#include <sstream>
#include <time.h>

#include "vec.h"
#include "FluidGrid.h"
#include "Texture.h"
#include "drawing.h"
#include "Menu.h"
#include "Timer.h"
#include "GameMode.h"
#include "Input.h"

// Algorithms from "Real Time Fluid Dynamics for Games" by Jos Stam.

using std::list;

static int XRES = 1024;
static int YRES = 768;

float TIMESTEP;
bool QUIT = false;


static FontRenderer* FONTRENDERER;

vec MOUSE;
vec JOYSTICK;

void set_resolution()
{
#ifdef NDEBUG
	SDL_Surface* screen = 
		SDL_SetVideoMode(XRES, YRES, 0, SDL_FULLSCREEN | SDL_OPENGL);
#else
	SDL_Surface* screen =
		SDL_SetVideoMode(XRES, YRES, 0, SDL_OPENGL);
#endif
	if (screen == 0) {
		std::cerr << "Failed to set video mode: " << SDL_GetError();
		exit(1);
	}

	glViewport(0,0,XRES,YRES);
}

void init_sdl() 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) == -1) {
		std::cout << "Couldn't initialize SDL: " << SDL_GetError() << "\n";
	}

	set_resolution();

	int sticks = SDL_NumJoysticks();
	for(int i = 0; i < sticks; i++) {
		if(SDL_JoystickOpen(i)) {
			SDL_JoystickEventState(1);
			break;
		}
	}

	SDL_ShowCursor(0);

	FONTRENDERER = new FontRenderer;

	glEnable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, W, 0, H);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void events(GameMode* game) 
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (game && game->events(&e)) continue;
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
			QUIT = true;
		}
	}
}

void run(GameMode* game)
{
	Timer fpstimer;
	int frames = 0;
	Timer timer;

	MOUSE = vec();
	QUIT = false;
	while (!QUIT) {
		events(game);
		game->step();

		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		game->draw();
		SDL_GL_SwapBuffers();

		frames++;
		if (frames % 30 == 0) {
			std::cout << "FPS: " << 30 / fpstimer.get_diff() << "\n";
		}
		
		TIMESTEP = timer.get_diff();
		float dt = TIMESCALE * TIMESTEP;
		if (dt < DT) {
			SDL_Delay(int(1000*(DT - dt)/TIMESCALE));
		}

		SDL_Delay(0);
	}
}

class GalagaMenuItem : public MenuItem {
public:
	GalagaMenuItem() : MenuItem("One Player") { }
	void action(Menu* m) {
		std::list<Input*> inputs;
		inputs.push_back(new WASDInput);
		inputs.push_back(new ArrowsInput);
		inputs.push_back(new JoystickInput);
		inputs.push_back(new MouseInput);
		GameMode* mode = make_GalagaMode(new AddInput(inputs));
		run(mode);
		delete mode;
		m->reinit();
	}
	bool color() { return false; }
};

class DuelMenuItem : public MenuItem {
public:
	DuelMenuItem() : MenuItem("Two Players") { }
	void action(Menu* m) { 
		std::list<Input*> posinputs;
		posinputs.push_back(new WASDInput);
		posinputs.push_back(new JoystickInput);
		std::list<Input*> neginputs;
		neginputs.push_back(new ArrowsInput);
		neginputs.push_back(new MouseInput);
		GameMode* mode = make_DuelMode(new AddInput(posinputs), new AddInput(neginputs));
		run(mode); 
		delete mode;
		m->reinit(); 
	}
	bool color() { return true; }
};

class ResolutionMenuItem : public MenuItem {
public:
	ResolutionMenuItem() : MenuItem(""), modes_(SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_OPENGL)) { 
		for (idx_ = 0; modes_[idx_]; idx_++) {
			if (modes_[idx_]->w == XRES && modes_[idx_]->h == YRES) {
				break;
			}
		}
		if (!modes_[idx_]) idx_ = 0;
		parity_ = (idx_+1) % 2;
	}

	string get_text() const {
		std::stringstream ss;
		ss << modes_[idx_]->w << "x" << modes_[idx_]->h;
		return ss.str();
	}

	void action(Menu*) {
		XRES = modes_[idx_]->w;
		YRES = modes_[idx_]->h;
		set_resolution();
	}
	
	void right_action(Menu*) {
		if (idx_ == 0) return;
		idx_--;
	}

	void left_action(Menu*) {
		if (!modes_[idx_+1]) return;
		idx_++;
	}
	
	bool color() { return (idx_ + parity_) % 2; }
private:
	SDL_Rect** modes_;
	int idx_;
	int parity_;
};

class OptionsMenuItem : public MenuItem {
public:
	OptionsMenuItem() : MenuItem("Options"), options_menu(FONTRENDERER) {
		options_menu.add_item(new ResolutionMenuItem);
	}
	void action(Menu* m) {
		options_menu.run();
	}
	bool color() { return false; }
private:
	Menu options_menu;
};

class HighScoresMenuItem : public MenuItem {
public:
	HighScoresMenuItem() : MenuItem("High Scores") { }
	void action(Menu* m) {
		GameMode* mode = make_HighScoreMode();
		run(mode);
		delete mode;
		m->reinit();
	}
	bool color() { return true; }
};

class QuitMenuItem : public MenuItem {
public:
	QuitMenuItem() : MenuItem("Quit") { }
	void action(Menu*) { SDL_Quit(); exit(0); }
	bool color() { return false; }
};

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	init_sdl();

	Menu main_menu(FONTRENDERER);
	main_menu.add_item(new GalagaMenuItem);
	main_menu.add_item(new DuelMenuItem);
	main_menu.add_item(new OptionsMenuItem);
	main_menu.add_item(new HighScoresMenuItem);
	main_menu.add_item(new QuitMenuItem);

	main_menu.run();
	
	delete FONTRENDERER;
	SDL_Quit();

	return 0;
}
