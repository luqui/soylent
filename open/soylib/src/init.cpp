#include "soy/init.h"
#include "soy/error.h"
#include "soy/Viewport.h"

void SoyInit::init()
{
	SDL_Init(init_flags_) ONERROR DIE("Couldn't initialize SDL: " + SDL_GetError());

	Uint32 fsflag = fullscreen_ ? SDL_FULLSCREEN : 0;
	
	if (init_flags_ & SDL_INIT_VIDEO) {
		SDL_Rect** modes = SDL_ListModes(NULL, fsflag | SDL_OPENGL);
		SDL_Rect mode = {};
		if (modes == NULL) {
			DIE("No video modes available.");
		}
		else if (modes == (SDL_Rect**)(-1)) {
			// any mode is valid
            if (dwidth_ || dheight_) {
                mode.w = dwidth_;
                mode.h = dheight_;
            }
            else {
                mode.w = 1024;
                mode.h = 768;
            }
		}
		else {
			Uint32 bestarea = 0;
			SDL_Rect* bestmode = 0;
			for (SDL_Rect** i = modes; *i; i++) {
				Uint32 area = (*i)->w * (*i)->h;
                if (dwidth_ && dheight_ && (*i)->w == dwidth_ && (*i)->h == dheight_) {
                    bestmode = *i;
                    break;
                }
				if (area > bestarea) {
					bestarea = area;
					bestmode = *i;
				}
			}

			if (bestmode == NULL) DIE("No mode with nonzero area (very strange)");
			mode = *bestmode;
		}

        surface_ = SDL_SetVideoMode(mode.w, mode.h, 0, fsflag | SDL_OPENGL);
		if (surface_ == NULL) DIE("Failed to set video mode: " + SDL_GetError());
		
		width_ = mode.w;
		height_ = mode.h;
	}
	inited_ = true;
}

Viewport SoyInit::pixel_view() const {
	return Viewport::from_bounds(vec2(0,0), vec2(width(), height()));
}

void SoyInit::quit()
{
	SDL_Quit();
}
