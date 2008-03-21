#ifndef __GAMEMODE_H__
#define __GAMEMODE_H__

#include <SDL.h>

class GameMode {
public:
    virtual ~GameMode() { }

    virtual void step() = 0;
    virtual void draw() const = 0;
    virtual bool events(const SDL_Event& e) = 0;
};

#endif
