#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "config.h"

class Clock
{
private:
	Uint32 startTicks;
	Uint32 stopTicks;
	int lastClock;
	int stopCounter;

public:
	Clock();
	int clockNum();
	void start();//isn't startGameClock() what you want?
	void stop();//isn't stopGameClock() what you want?
	void reset();
	void draw(SDL_Surface* drawSurface);
	void redraw(SDL_Surface* drawSurface);
	bool inClock(int x, int y);
};

#endif
