#include "Menu.h"
#include "drawing.h"
#include "util.h"
#include <iostream>
#include <cmath>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

static int read_pixel(SDL_Surface* surf, int x, int y) {
	Uint8* ptr = &((Uint8*)surf->pixels)[y * surf->pitch + surf->format->BytesPerPixel * x];
	if ((Uint8*)ptr > (Uint8*)surf->pixels + surf->h * surf->pitch
	  ||(Uint8*)ptr < (Uint8*)surf->pixels) return 0;
	switch (surf->format->BitsPerPixel) {
		case 8: return *ptr;
		case 16: return *(Uint16*)ptr;
		case 32: return *(Uint32*)ptr;
		default: abort();
	}
}

const int MENUBOUNDX = 20;
const int MENUBOUNDY = 4;

static bool BITCHED = false;

void FontRenderer::step()
{
	const float factor = pow(float(0.6), float(DT));
	for (int x = 0; x < MENUW; x++) {
		for (int y = 0; y < MENUH; y++) {
			float den = grid_.get_density_direct(x,y);
			grid_.set_density_direct(x,y,den * factor);
		}
	}
			
	fallback_ = false;
	
	SDL_Color white = { 255, 255, 255 };
	SDL_Color black = { 0, 0, 0 };
	SDL_Surface* surf = TTF_RenderText_Shaded(font_, text_.c_str(), white, black);

	if (surf == 0) {  // Luke's desktop is being a bitch, so we must fallback
	                  // if TTF isn't working
		
		if (!BITCHED) {
			std::cout << "Couldn't render text: " << TTF_GetError() << "\n";
			BITCHED = true;
		}
		fallback_ = true;
		return;
	}

	for (int y = 0; y < MENUH; y++) {
		for (int x = 0; x < MENUW; x++) {
			int picx = int(float(surf->w) * (x - MENUBOUNDX) / (MENUW - 2*MENUBOUNDX));
			int picy = int(float(surf->h) * (MENUH - y) / MENUH);
			if (picx >= 0 && picx < surf->w && picy >= 0 && picy < surf->h) {
				int pix = read_pixel(surf,picx,picy);
				if (pix) {
					grid_.add_density(vec(x,y), den_);
					grid_.add_velocity(vec(x,y), vel_);
				}
			}
		}
	}

	SDL_FreeSurface(surf);

	grid_.step_velocity();
	grid_.step_density();
}

void FontRenderer::draw() const
{
	if (fallback_) {
		glRasterPos2f(W/3, H/2);
		glColor3f(1,1,1);
		draw_string(text_);
	}
	else {
		draw_board(&grid_, MENUW, MENUH);
	}
}

void FontRenderer::reinit()
{
	grid_.erase();
}

void Menu::events() 
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_ESCAPE: {
					quit(); 
					break;
				}
				case SDLK_DOWN: {
					items_t::iterator it = item_;
					++it;
					if (it != items_.end()) { ++item_;  }
					break;
				}
				case SDLK_UP: {
					if (item_ != items_.begin()) { --item_; }
					break;
				}
				case SDLK_LEFT: {
					(*item_)->left_action(this);
					break;
				}
				case SDLK_RIGHT: {
					(*item_)->right_action(this);
					break;
				}
				case SDLK_SPACE:
				case SDLK_KP_ENTER:
				case SDLK_RETURN: {
					(*item_)->action(this);
				}
				default: break;
			}
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_JOYBUTTONDOWN) {
			(*item_)->action(this);
		}
	}
}

void Menu::run()
{
	quit_ = false;
	while (!quit_) {
		events();

		std::string arrow;
		{
			int bits = 0;
			items_t::iterator it = item_;
			++it;
			if (item_ != items_.begin()) bits |= 1;   // can go up
			if (it != items_.end())      bits |= 2;   // can go down

			switch (bits) {
				case 0: arrow = "  ";  break;
				case 1: arrow = " @";  break;
				case 2: arrow = " !";  break;
				case 3: arrow = " #";  break;
			}
		}
		
		std::string text = (*item_)->get_text() + arrow;
		renderer_->set_text(text.c_str(), 
							((*item_)->color() ? vec(0.006,0.016) : vec(-0.006,-0.016)),
							((*item_)->color() ? 0.015 : -0.015));
							
		renderer_->step();
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		renderer_->draw();
		SDL_GL_SwapBuffers();
	}
}

