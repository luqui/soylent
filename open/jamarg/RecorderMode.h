#ifndef __RECORDERMODE_H__
#define __RECORDERMODE_H__

#include "GameMode.h"
#include <string>
#include <fstream>
#include <SDL.h>
#include <SDL_mixer.h>

class RecorderMode : public GameMode {
public:
    RecorderMode(const std::string& mp3, const std::string& beatmap);
    ~RecorderMode();

    void step();
    void draw() const;
    bool events(const SDL_Event& e);
    
private:
    Uint32 starttime_;
    std::ofstream beatmap_;
    Mix_Music* music_;
};

#endif
