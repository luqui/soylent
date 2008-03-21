#include <soy/init.h>
#include <soy/Timer.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Element.h"
#include "Field.h"

const double DT = 1/30.0;

SoyInit INIT;
Field FIELD;

void init() {
    INIT.init();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,32,0,24);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    load_terrain_textures();
    FIELD.randomize();
}

void step() {
    for (int i = 0; i < 4; i++) {
        FIELD.diffuse(1.0, DT/4);
        FIELD.advect(DT/4);
    }
}

void draw() {
    FIELD.texdraw();
}


void events() {
    SDL_Event e;
    static double oldx = 0;
    static double oldy = 0;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) { exit(0); }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) { exit(0); }
        if (e.type == SDL_MOUSEMOTION) {
            double x = e.motion.x * 32.0 / 1024;
            double y = (768 - e.motion.y) * 24.0 / 768;
            if (e.motion.state & SDL_BUTTON(1)) {
                FIELD.move(oldx,oldy,x,y);
            }
            oldx = x;
            oldy = y;
        }
    }

    {
        int mx,my;
        Uint8 buts = SDL_GetMouseState(&mx,&my);
        if (buts & SDL_BUTTON(3)) {
            double x = mx * 32.0 / 1024;
            double y = (768 - my) * 24.0 / 768;
            double dt = DT;
            if (buts & SDL_BUTTON(1)) dt = 1;
            FIELD.erase(x,y,dt);
        }
    }
}

int main() {
    srand(time(NULL));
    init();

    FrameRateLockTimer timer(DT);

    while (true) {
        timer.lock();
        step();
        events();

        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();
    }
}
