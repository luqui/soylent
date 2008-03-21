#ifndef __GAMEMODE_H__
#define __GAMEMODE_H__

class GameMode
{
public:
    virtual ~GameMode() { }

    virtual void step() = 0;
    //virtual void draw() const = 0;
};

#endif
