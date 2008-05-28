#ifndef __MENU_H__
#define __MENU_H__

#include "tweak.h"
#include <SDL_ttf.h>

#include <string>
#include <list>
#include <iostream>
#include "FluidGrid.h"
#include "Timer.h"

using std::string;
using std::list;

extern const int W, H;

const int MENUW = W;
const int MENUH = H;

class FontRenderer {
public:
	FontRenderer() : 
			 grid_(MENUW,MENUH,vec(0,0),vec(MENUW,MENUH),0,1e-4),
			 den_(1), fallback_(false)
	{
		if (!TTF_WasInit() && TTF_Init() == -1) {
			std::cout << "Couldn't initialize font library: " << TTF_GetError() << "\n";
			SDL_Quit();
			exit(1);
		}
		font_ = TTF_OpenFont(ICHOR_DATADIR "/larabief.ttf", 48);
		if (font_ == 0) {
			std::cout << "Couldn't open font: " << TTF_GetError() << "\n";
			SDL_Quit();
			exit(1);
		}
	}

	~FontRenderer() {
		TTF_CloseFont(font_);
	}
	
	void set_text(const string& text, const vec& vel, float den) { 
		text_ = text; 
		vel_ = vel;
		den_ = den;
	}
	
	void step();
	void draw() const;
	void reinit();

private:
	Timer timer_;
	FluidDensityGrid grid_;
	TTF_Font* font_;
	string text_;
	vec vel_;
	float den_;
	bool fallback_;
};


class Menu;
class MenuItem
{
public:
	MenuItem(string text) : text_(text) { }
	virtual ~MenuItem() { }

	virtual void action(Menu* menu) { }
	virtual void left_action(Menu* menu) { }
	virtual void right_action(Menu* menu) { }
	virtual bool color() { return true; }

	virtual string get_text() const { return text_; }

private:
	string text_;
};

class Menu
{
public:
	Menu(FontRenderer* renderer, bool parity = 0) : 
		     item_(items_.end()), 
			 renderer_(renderer),
			 quit_(true)
	{ }

	~Menu() {
		for (items_t::iterator i = items_.begin(); i != items_.end(); ++i) {
			delete *i;
		}
	}

	void add_item(MenuItem* item) { 
		items_.push_back(item); 
		item_ = items_.begin();
	}

	void events();
	void run();
	void quit() { quit_ = true; }
	void reinit() { renderer_->reinit(); }

private:
	typedef list<MenuItem*> items_t;
	items_t items_;
	items_t::iterator item_;

	FontRenderer* renderer_;

	bool quit_;
};

#endif
