#ifndef __SOYLIB_TIMER_H__
#define __SOYLIB_TIMER_H__

#include <SDL.h>
#include "soy/error.h"

class Timer
{
public:
	Timer() : last_ticks_(0), inited_(false) { }

	// The timer must be initialized for two reasons:
	//   (1) We shouldn't call SDL_GetTicks before SDL_Init,
	//       so if we did this in the constructor, a global
	//       timer would break the program.
	//   (2) We shouldn't be measuring the time of the
	//       init code of the program in that case, either.
	void init() {
		last_ticks_ = SDL_GetTicks();
		inited_ = true;
	}

	double get_time_diff() {
		if (!inited_) DIE("Timer not initialized!");
		Uint32 ticks = SDL_GetTicks();
		double ret = double(ticks - last_ticks_) / 1000;
		last_ticks_ = ticks;
		return ret;
	}

private:
	Uint32 last_ticks_;
	bool inited_;
};

class FrameRateLockTimer
{
public:
	FrameRateLockTimer(double dt) : inited_(false), dt_(dt), correction_(0) { }

	void lock() { 
		if (!inited_) {
			ticks_ = SDL_GetTicks();
			inited_ = true;
		}
		else {
			Uint32 newticks = SDL_GetTicks();
			double diff = 1e-3 * (newticks - ticks_);
			int delayticks = int(1000 * (dt_ - diff)) - correction_;
			if (delayticks > 0) {
				SDL_Delay(delayticks);
			}
			ticks_ = SDL_GetTicks();
			// in case we delayed too long:
			correction_ = (ticks_ - newticks) - int(1000 * (dt_ - diff));
			if (correction_ < 0) correction_ = 0;
		}
	}
	
private:
	bool inited_;
	const double dt_;

	Uint32 ticks_;
	Uint32 correction_;
};

class FrameRateMeasurer
{
public:
	FrameRateMeasurer() : frames_(30), framerate_(0) {
		timer_.init();
	}

	void step() {
		frames_--;
		if (frames_ == 0) {
			framerate_ = 30 / timer_.get_time_diff();
			frames_ = 30;
		}
	}

	float get_framerate() const {
		return framerate_;
	}
private:
	Timer timer_;
	int frames_;
	double framerate_;
};

#endif
