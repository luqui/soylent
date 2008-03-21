#include "Menu.h"
#include "drawing.h"
#include "util.h"
#include "tweak.h"
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
static string OLD_TEXT = "";

void FontRenderer::step()
{
	/*const float factor = pow(float(0.6), float(DT));
	for (int x = 0; x < MENUW; x++) {
		for (int y = 0; y < MENUH; y++) {
			float den = grid_.get_density_direct(x,y);
			grid_.set_density_direct(x,y,den * factor);
		}
	}*/
			
	fallback_ = false;
	
	if (OLD_TEXT != text_) {
		OLD_TEXT = text_;
		g_pFlo->ResetBoundary();

		SDL_Color white = { 255, 255, 255 };
		SDL_Surface* surf = TTF_RenderText_Blended(font_, text_.c_str(), white);
		Texture* tex = new Texture("", surf);

		if (surf == 0) {  // Luke's desktop is being a bitch, so we must fallback
						// if TTF isn't working
			
			if (!BITCHED) {
				std::cout << "Couldn't render text: " << TTF_GetError() << "\n";
				BITCHED = true;
			}
			fallback_ = true;
			return;
		}

		int bottom = MENUBOUNDY;
		int left = MENUBOUNDX;
		grid_.add_boundary_tex(vec(left, bottom), tex);

		SDL_FreeSurface(surf);

		delete tex;
	}

	for(int x = W/12; x < W; x+=W/6) {
		vec pos = vec(x, H - float(H)/30);
		grid_.add_density(pos, -5, 1.0/float(H));
		grid_.add_velocity(pos, vec(0, -10), 1.0/float(H));

		vec pos2 = vec(x, float(H)/30);
		grid_.add_density(pos2, 5, 1.0/float(H));
		grid_.add_velocity(pos2, vec(0, 10), 1.0/float(H));
	}

	/*float color[3] = {0, .0005, 0};
	float position[2] = {0.5, 0.5};
	static int suckage = 100;
	if(suckage % 5 == 0) {
		g_pFlo->AddDensityImpulse(color, position, 100);
		suckage = 0;
	}
	suckage++;*/

	grid_.step();
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
					OLD_TEXT = "";
					(*item_)->action(this);
				}
				default: break;
			}
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			OLD_TEXT = "";
			(*item_)->action(this);
		}
		else if (e.type == SDL_JOYBUTTONDOWN && e.jbutton.state == SDL_PRESSED) {
			if (e.jbutton.button == 1) {
				quit(); 
				break;
			}
			else {
				OLD_TEXT = "";
				(*item_)->action(this);
			}
		}
		else if (e.type == SDL_JOYAXISMOTION) {
			static bool joyFlgX = false; //this flag keeps track of whether the joystick has returned to a neutral position
			static bool joyFlgY = false;
			if (e.jaxis.axis == 0) {
				float temp = float(e.jaxis.value) / 32768;
				if (temp > .5 && !joyFlgX) {//right
					(*item_)->right_action(this);
					joyFlgX = true;
					break;
				}
				else if (temp < -.5 && !joyFlgX) {//left
					(*item_)->left_action(this);
					joyFlgX = true;
					break;
				}
				else if (temp < .25 && temp > -.25) {
					joyFlgX = false;
				}
			}
			else if (e.jaxis.axis == 1) {
				float temp = float(e.jaxis.value) / 32768;
				if (temp < -.5 && !joyFlgY) {//up
					if (item_ != items_.begin()) { --item_; }
					joyFlgY = true;
					break;
				}
				else if (temp > .5 && !joyFlgY) {//down
					items_t::iterator it = item_;
					++it;
					if (it != items_.end()) { ++item_;  }
					joyFlgY = true;
					break;
				}
				else if (temp < .25 && temp > -.25) {
					joyFlgY = false;
				}
			}
		}
	}
}

void Menu::run()
{
	Timer fpstimer;
	int frames = 0;
	Timer timer;
	float tstep;

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
				case 0: arrow = " <";  break;
				case 1: arrow = " ^";  break;
				case 2: arrow = " v";  break;
				case 3: arrow = " >";  break;
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

		frames++;
		if (frames % 30 == 0) {
			std::cout << "FPS: " << 30 / fpstimer.get_diff() << "\n";
		}
		
		tstep = timer.get_diff();
		if (tstep < DT/TIMESCALE) {
			SDL_Delay(int(1000*(DT/tstep - tstep)));
			timer.get_diff();  // discard that time
		}

		SDL_Delay(0);
	}
}

