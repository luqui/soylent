#include "tweak.h"
#include "ParticleField.h"
#include "Terrain.h"
#include <soy/init.h>
#include <soy/Viewport.h>
#include <soy/Timer.h>
#include <list>

SoyInit INIT;
ParticleField PARTICLES(PARTICLE_FIELD_BIN_SIZE);
Viewport VIEWPORT = Viewport::from_center_dim(vec2(0,0), vec2(320,240));

typedef std::list<Cell*> cells_t;
cells_t CELLS;

bool DRAW_CELLS = true;
bool DRAW_PARTICLES = true;

std::list<Terrain> TERRAIN;

void init() {
    INIT.init();
    VIEWPORT.activate();
}

void init_terrain() {
    TERRAIN.push_back(Terrain(VIEWPORT, 
                              std::vector<double>(NUM_ENERGY_TYPES, 1.0/NUM_ENERGY_TYPES),
                              DT/100));
}

void draw() {
    if (DRAW_CELLS) {
        for (cells_t::iterator i = CELLS.begin(); i != CELLS.end(); ++i) {
            (*i)->draw();
        }
    }

    if (DRAW_PARTICLES) {
        PARTICLES.draw();
    }
}

void step() {
    PARTICLES.step();
    for (cells_t::iterator i = CELLS.begin(); i != CELLS.end();) {
        PARTICLES.absorb(*i);
        (*i)->step();
        (*i)->wrap_position(VIEWPORT);

        if (!(*i)->alive()) {
            cells_t::iterator j = i;
            ++i;
            (*j)->die();
            delete *j;
            CELLS.erase(j);
        }
        else {
            ++i;
        }
    }

    for (std::list<Terrain>::iterator i = TERRAIN.begin(); i != TERRAIN.end(); ++i) {
        i->plot();
        i->mutate();
    }

    static double celltime = 0;
    while (celltime <= 0) {
        vec2 p = random_vector(VIEWPORT);
        Cell* c = new Cell(p);
        c->randomize();
        CELLS.push_back(c);

        celltime += 5;
    }
    celltime -= DT;
}

void events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            INIT.quit();
            exit(0);
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_c) {
            DRAW_CELLS = !DRAW_CELLS;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p) {
            DRAW_PARTICLES = !DRAW_PARTICLES;
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
            for (std::list<Terrain>::iterator i = TERRAIN.begin(); i != TERRAIN.end(); ++i) {
                i->alter_speed(sqrt(2.0));
            }
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
            for (std::list<Terrain>::iterator i = TERRAIN.begin(); i != TERRAIN.end(); ++i) {
                i->alter_speed(sqrt(0.5));
            }
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_n) {
            for (int i = 0; i < 10; i++) {
                vec2 p = random_vector(VIEWPORT);
                Cell* c = new Cell(p);
                c->randomize();
                CELLS.push_back(c);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    FrameRateLockTimer timer(DT);
    init();
    init_terrain();

    while (true) {
        timer.lock();
        events();
        step();

        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();
    }
}
