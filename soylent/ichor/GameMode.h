#ifndef __GAMEMODE_H__
#define __GAMEMODE_H__

#include "tweak.h"
#include <SDL.h>

class GameMode
{
public:
	virtual ~GameMode() { }

	virtual void draw() const = 0;
	virtual void step() = 0;
	virtual bool events(SDL_Event* e) = 0;
};

class Input;
class ReaderInput;
GameMode* make_GalagaMode(Input*);
GameMode* make_GalagaMode(Input*, unsigned int seed);
GameMode* make_GalagaMode(ReaderInput*);
GameMode* make_DuelMode(Input*, Input*);
GameMode* make_HighScoreMode();

#endif
