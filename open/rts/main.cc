#include <list>
#include <iostream>
#include <soy/init.h>
#include <soy/Timer.h>
#include <soy/Viewport.h>
#include <ruby.h>
#include "config.h"
#include "util.h"
#include "Board.h"
#include "Unit.h"
#include "Units/Hummer.h"
#include "TerrainGenerator.h"
#include "Dust.h"
#include "Scheduler.h"
#include "UI.h"
#include "ruby_interop.h"

typedef std::list<Unit*> units_t;
units_t UNITS;
Dust DUST;
Scheduler SCHEDULER;
UI* MAINUI = 0;
num GAMETIME = 0;

static bool do_fullscreen = true;

const int WIDTH = 256, HEIGHT = 192;

SoyInit INIT;
Board BOARD(WIDTH, HEIGHT);

void init() {
	if (do_fullscreen) INIT.set_fullscreen();
	INIT.init();
	glEnable(GL_TEXTURE_2D);
	srand(time(NULL));
	MAINUI = make_MainUI();
}

void draw() {
	MAINUI->draw();
}

void step() {
	GAMETIME += DT;
	
	for (units_t::const_iterator i = UNITS.begin(); i != UNITS.end(); ++i) {
		(*i)->step();
	}
	
	DUST.step();

	MAINUI->step();
}

void events() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (MAINUI->events(&e)) break;

		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE
		 || e.type == SDL_QUIT) {
			SDL_Quit();
			exit(0);
		}
	}
}

// provided as a callback for ruby after it is done loading
VALUE main_game_loop(VALUE self) {
	Timer timer;
	FrameRateMeasurer fps;

	TerrainGenerator().generate();
	
	UNITS.push_back(new HummerUnit(vec2(16.5, 12.5)));
	UNITS.push_back(new HummerUnit(vec2(19.5, 14.5)));
	UNITS.push_back(new HummerUnit(vec2(22.5, 16.5)));
	UNITS.push_back(new HummerUnit(vec2(14.5, 11.5)));
	UNITS.push_back(new HummerUnit(vec2(17.5, 13.5)));
	UNITS.push_back(new HummerUnit(vec2(20.5, 15.5)));
	UNITS.push_back(new HummerUnit(vec2(12.5, 10.5)));
	UNITS.push_back(new HummerUnit(vec2(12.5, 12.5)));
	UNITS.push_back(new HummerUnit(vec2(15.5, 14.5)));

	timer.init();
	num idle = 0;
	int fpsprinter = 0;
	while (true) {
		idle += DT;

		events();
		step();

		glClear(GL_COLOR_BUFFER_BIT);
		draw();
		SDL_GL_SwapBuffers();
		fps.step();
		if (++fpsprinter % 100 == 0) {
			std::cout << fps.get_framerate() << " FHz\n";
		}
		
		idle -= timer.get_time_diff();
		if (idle > 0) {
			SCHEDULER.compute(idle);
			idle -= timer.get_time_diff();
			if (idle > 0) {
				SDL_Delay(int(1000 * idle));
				idle = 0;
			}
		}
		else {
			idle = 0;  // frame floor
			SCHEDULER.compute(0);  // *some* work needs to get done
		}
		timer.get_time_diff();  // discard this time
	}
	return Qnil;
}

void proc_args(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "-nofs" || arg == "--nofs") {
			do_fullscreen = false;
		}
		else {
			DIE("Unknown command line argument " + arg);
		}
	}
}

int main(int argc, char** argv) {
	proc_args(argc, argv);
	init();
	ruby_init();
	ruby_script("RTS");
	rb_load_file("boot.rb");
	rb_define_global_function("main_game_loop", reinterpret_cast<VALUE (*)(...)>(main_game_loop), 0);
	setup_ruby_wrappers();
	ruby_run();
	rb_exit(0);
}
