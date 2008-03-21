#include "RecorderMode.h"
#include <SDL_mixer.h>
#include <soy/error.h>
#include <iostream>

RecorderMode::RecorderMode(const std::string& mp3, const std::string& beatmap)
{
    music_ = Mix_LoadMUS(mp3.c_str());
    if (!music_) {
        DIE("Couldn't load " + mp3 + ": " + Mix_GetError());
    }
    beatmap_.open(beatmap.c_str());
    if (!beatmap_.good()) {
        DIE("Couldn't open " + beatmap);
    }
    Mix_PlayMusic(music_, 0);
    starttime_ = SDL_GetTicks();
}

RecorderMode::~RecorderMode()
{
    std::cout << "Closing\n";
    Mix_FreeMusic(music_);
    beatmap_.close();
}

void RecorderMode::step()
{ }

void RecorderMode::draw() const
{ }

bool RecorderMode::events(const SDL_Event& e) 
{
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        beatmap_ << (SDL_GetTicks() - starttime_) << "\n";
        return true;
    }
    return false;
}
