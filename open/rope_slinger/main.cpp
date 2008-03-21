#include <iostream>

#include "common.h"
#include "player.h"
#include "geometry.h"
#include "objects.h"
#include "level.h"
#include "effects.h"

num STEP = 1/60.0;
num OVERSTEP = 0;

int P1_SCORE = 0;
int P2_SCORE = 0;

void setup_gfx() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);
    SDL_SetVideoMode(PIXEL_WIDTH, PIXEL_HEIGHT, 24, 
                     SDL_OPENGL | (SDL_FULLSCREEN * FULLSCREEN)) >= 0
        || DIE("Couldn't set video mode: " + SDL_GetError());
    
    SDL_ShowCursor(0);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 4, 0, 3);
    glMatrixMode(GL_MODELVIEW);

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
}

num get_time_diff() {
    static Uint32 prev = SDL_GetTicks();
    Uint32 time = SDL_GetTicks();
    num ret = num(time - prev) / 1000.0;
    prev = time;
    return ret;
}

bool intro = true;
EffectsManager* INTROFX = 0;

void start_game(string level) {
    LEVEL = new Level;
    LEVEL->load_level(level);
    MOUSE_FOCUS = LEVEL->player;
}

void step(string level) {
    if (intro) {
        INTROFX->step();
        if (INTROFX->done()) {
            delete INTROFX;
            INTROFX = 0;
            intro = false;
            
            Mix_FadeOutMusic(4000);
            start_game(level);
        }
    }
    else {
        LEVEL->step();
        if (LEVEL->frozen()) {
            bool restart = true;
            if (LEVEL->p1->dead() && LEVEL->p2->dead()) ;
            else if (LEVEL->p1->dead()) P2_SCORE++;
            else if (LEVEL->p2->dead()) P1_SCORE++;
            else restart = false;

            if (restart) {
                delete LEVEL;
                LEVEL = new Level;
                LEVEL->load_level(level);
                MOUSE_FOCUS = LEVEL->player;
            }
        }
    }
}

void draw(num overstep) {
    if (intro) {
        INTROFX->draw();
    }
    else {
        if (!LEVEL->frozen()) OVERSTEP = overstep; else OVERSTEP = 0;
        LEVEL->draw();
    }
}

int main(int argc, char** argv) {
    setup_gfx();

    bool do_intro = true;
    string level;
    if (argc == 2) {
        level = argv[1];  do_intro = false;
    }
    else {
        level = "levels/walls.lvl";
    }

    Mix_Music* music = Mix_LoadMUS("media/title.mp3");
    music || DIE("Couldn't load music: " + Mix_GetError());
    if (do_intro) {
        Mix_PlayMusic(music, 1);

        INTROFX = new EffectsManager;
        MOUSE_FOCUS = INTROFX;
        INTROFX->add(
                new SeqEffect(
                    new DelayEffect(4),
                    new SeqEffect(
                        new SeqEffect(
                            new FadeImageEffect(
                                    "media/chronic.png",
                                    vec(1.5, 1.25), vec(2.5, 1.75),
                                    4, 4, 3),
                            new FadeImageEffect(
                                    "media/soylent.png",
                                    vec(1.0, 1.25), vec(3.0, 1.75),
                                    4, 4, 3)),
                        new SeqEffect(
                            new DelayEffect(2),
                            new FadeImageEffect(
                                    "media/title.png",
                                    vec(0, 0.5), vec(4, 2.5),
                                    4, 4, 4)))));
    }
    else { 
        intro = false;
        start_game(level); 
    }

    num overstep = 0;
    
    while (true) {
        events();
        step(level);
        overstep -= STEP;

        // avoid degenerate behavior for expensive step()s.
        if (overstep > STEP) overstep = STEP;
        while (overstep <= STEP) {
            glClear(GL_COLOR_BUFFER_BIT);
            glLoadIdentity();
            draw(overstep);
            SDL_GL_SwapBuffers();
            overstep += get_time_diff();
        }
    }

    Mix_FreeMusic(music);

    delete LEVEL;
    quit();
}
