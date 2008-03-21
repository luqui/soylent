#ifndef __PLAYBACKMODE_H__
#define __PLAYBACKMODE_H__

#include "GameMode.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <fstream>

class PlaybackMode : public GameMode
{
public:
    PlaybackMode(const std::string& mp3, const std::string& beatmap);
    ~PlaybackMode();

    void step();
    void draw() const;
    bool events(const SDL_Event& e);

private:
    Mix_Music* music_;
    std::ifstream beatmap_;
    Uint32 starttime_;
    Uint32 nextbeat_;
    bool flashy_;
};

#endif
