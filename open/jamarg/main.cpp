#include "GameMode.h"
#include "RecorderMode.h"
#include "PlaybackMode.h"
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <soy/init.h>
#include <soy/Timer.h>
#include <SDL_mixer.h>
#include <cstdlib>

const double DT = 1.0/30.0;
SoyInit INIT;
GameMode* MODE = 0;

void init() {
    INIT.init();
    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024);
}

void quit() {
    delete MODE;
    Mix_CloseAudio();
    INIT.quit();
    exit(0);
}

void step() {
    MODE->step();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    MODE->draw();
    SDL_GL_SwapBuffers();
}

void events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (MODE->events(e)) {
            continue;
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            quit();
        }
    }
}

int main(int argc, char** argv) {
    init();

    if (argc != 3) {
        DIE("Bad argument format, biatch");
    }

    if (argv[1] == std::string("-r")) {
        MODE = new RecorderMode(argv[2], "beatmap.txt");
    }
    else if (argv[1] == std::string("-p")) {
        MODE = new PlaybackMode(argv[2], "beatmap.txt");
    }
    else {
        DIE("Whacchu talkin bout!");
    }
        
    FrameRateLockTimer timer(DT);
    while (true) {
        timer.lock();
        events();
        step();
        draw();
    }
}
