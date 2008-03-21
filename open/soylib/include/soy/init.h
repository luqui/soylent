#ifndef __SOYLIB_INIT_H__
#define __SOYLIB_INIT_H__

#include "soy/config.h"
#include <SDL.h>
#include <cstdlib>

#ifdef NDEBUG
#  define _SOY_FULLSCREEN_DEFAULT true
#else
#  define _SOY_FULLSCREEN_DEFAULT false
#endif

class Viewport;

class SoyInit {
public:
	SoyInit() : fullscreen_(_SOY_FULLSCREEN_DEFAULT), 
				init_flags_(SDL_INIT_VIDEO),
				inited_(false),
				width_(0), height_(0),
                dwidth_(0), dheight_(0) { }

	void set_fullscreen(bool fs = true) { fullscreen_ = fs; }
	void set_init_flags(Uint32 flags) { init_flags_ = flags; }

	int width()  const { if (!inited_) abort();  return width_; }
	int height() const { if (!inited_) abort();  return height_; }

    void set_res(int pixx, int pixy) {
        dwidth_ = pixx;
        dheight_ = pixy;
    }

	Viewport DLLEXPORT pixel_view() const;

	void DLLEXPORT init();
	void DLLEXPORT quit();

    SDL_Surface* get_surface() const { return surface_; }

private:
	bool fullscreen_;
	Uint32 init_flags_;
	bool inited_;
	int width_;
	int height_;
    int dwidth_;
    int dheight_;
    SDL_Surface* surface_;
};

#endif
