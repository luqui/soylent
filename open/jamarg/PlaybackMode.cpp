#include "PlaybackMode.h"
#include <soy/error.h>
#include <iostream>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

PlaybackMode::PlaybackMode(const std::string& mp3, const std::string& beatmap) 
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
    beatmap_ >> nextbeat_;
    flashy_ = false;
}

PlaybackMode::~PlaybackMode()
{
    std::cout << "Closing\n";
    Mix_FreeMusic(music_);
    beatmap_.close();
}

void PlaybackMode::step()
{
    flashy_ = false;
    Uint32 ticks = SDL_GetTicks() - starttime_;
    if (ticks > nextbeat_) {
        beatmap_ >> nextbeat_;
        flashy_ = true;
    }
}

void PlaybackMode::draw() const
{
    if (flashy_) {
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0,0,0,1);
    }
}

bool PlaybackMode::events(const SDL_Event& e)
{
    return false;
}
