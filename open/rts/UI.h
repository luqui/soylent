#ifndef __UI_H__
#define __UI_H__

#include <SDL.h>

class UI {
public:
	virtual ~UI() { }

	virtual void step() { }
	virtual void draw() const { }
	virtual bool events(SDL_Event* e) { return false; }
};

UI* make_MainUI();

#endif
