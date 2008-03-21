#include "soy/init.h"
#include "Cell.h"
#include "Dude.h"
#include "soy/Timer.h"
#include "MouseInterpreter.h"
#include "Viewport.h"
#include "Level.h"
#include "levels.h"
#include "Music.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

using std::vector;

float DT = 1/40.0;

Level* LEVEL = 0;
Dude* DUDE;
Timer TIMER;
vec2 MOUSE;
MouseInterpreter INTERP(vec2(1,4));
SoyInit INIT;
Viewport VIEWPORT(vec2(8,6), vec2(16, 12));
Music MUSIC;
bool FALLTHROUGH = false;

vec2 INITPOS = vec2(40, 4);

float FADEIN = 0;

void init() {
	INIT.set_fullscreen();
	INIT.set_init_flags(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	INIT.init();
	MUSIC.init();
	MUSIC.start();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 16, 0, 12);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
}

void reset() {
	delete LEVEL;
	LEVEL = make_level(1);
	VIEWPORT = Viewport(INITPOS, vec2(16,12));
	
	DUDE->set_pos(INITPOS);
	DUDE->set_vel(vec2(0,0));
	
	vec2 mpos = VIEWPORT.world_to_mouse(INITPOS);
	MOUSE = INITPOS;
	SDL_WarpMouse(int(mpos.x), int(mpos.y));
	INTERP = MouseInterpreter(MOUSE);
}

void events() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_KEYDOWN: {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					SDL_Quit();
					exit(0);
				}
				if (e.key.keysym.sym == SDLK_SPACE) {
					FALLTHROUGH = true;
				}
				if (e.key.keysym.sym == SDLK_m) {
					MUSIC.advance();
				}
			}
		}
	}

	int x, y;
	SDL_GetMouseState(&x, &y);
	MOUSE = VIEWPORT.mouse_to_world(vec2(x,y));
}

void draw() {
	VIEWPORT.center = vec2(DUDE->pos().x, DUDE->pos().y);
	VIEWPORT.make_active();
	LEVEL->cells()->draw();
	glBegin(GL_POINTS);
		glVertex2f(MOUSE.x, MOUSE.y);
	glEnd();

	if (FADEIN > 0) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS); {
			vec2 ll = VIEWPORT.center - VIEWPORT.dim;
			vec2 ur = VIEWPORT.center + VIEWPORT.dim;
			glColor4f(0,0,0,FADEIN*(1.1+sin(FADEIN))/15);
			glVertex2f(ll.x, ll.y);
			glColor4f(0,0,0,FADEIN*(1.1+sin(0.55*FADEIN))/15);
			glVertex2f(ll.x, ur.y);
			glColor4f(0,0,0,FADEIN*(1.1+sin(0.2*FADEIN))/15);
			glVertex2f(ur.x, ur.y);
			glColor4f(0,0,0,FADEIN*(1.1+sin(0.35*FADEIN))/15);
			glVertex2f(ur.x, ll.y);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	
	DUDE->draw();
}

void step() {
	MUSIC.step();
	INTERP.step();
	LEVEL->step();

	if (DUDE->pos().y < -60) {
		DUDE->set_pos(INITPOS);
		FALLTHROUGH = false;
		FADEIN = 28;
	}
	
	if (!FALLTHROUGH) {
		if (Cell* cell = LEVEL->cells()->get(int(DUDE->pos().x), int(DUDE->pos().y - 0.2))) {
			cell->trigger();
			if (cell->concrete()) {
				if (INTERP.jumps() > 0) {
					DUDE->set_vel(cell->jump_power(LEVEL->jump_power() * INTERP.jumps()) * vec2(0,1) + DUDE->vel());
				}
				DUDE->add_force(DUDE->vel() * vec2(-1,0));
			}
		}
	}

	if (DUDE->vel().norm() > 20) {
		DUDE->set_vel(20 * ~DUDE->vel());
	}

	DUDE->add_force(vec2(1,0) * INTERP.walks());
	DUDE->add_force(vec2(0,1) * INTERP.flies());
	
	Uint8* keys = SDL_GetKeyState(NULL);
	if (!keys[SDLK_g])  DUDE->add_force(LEVEL->gravity());

	FADEIN -= DT; if (FADEIN < 0) FADEIN = 0;

	DUDE->step();
}

int main() {
	init();
	DUDE = new Dude;
	reset();
	
	TIMER.init();
	while (true) {
		events();
		step();
		glClear(GL_COLOR_BUFFER_BIT);
		draw();
		SDL_GL_SwapBuffers();

		float diff = TIMER.get_time_diff();
		if (diff < DT) {
			SDL_Delay(int(1000 * (DT - diff)));
		}
	}
}
