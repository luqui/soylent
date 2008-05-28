#ifndef __TIMER_H__
#define __TIMER_H__

#include <SDL.h>
#include <cstdlib>

class Timer {
public:
	Timer() : ticks_(SDL_GetTicks()) { }
	
	float get_diff() {
		Uint32 newticks = SDL_GetTicks();
		float ret = (newticks - ticks_) / 1000.0;
		ticks_ = newticks;
		return ret;
	}
private:
	Uint32 ticks_;
};

#endif
