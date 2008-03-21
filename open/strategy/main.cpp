#include "config.h"
#include "Terrain.h"
#include "Soldier.h"
#include "Messages.h"
#include "Board.h"
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

void init_graphics() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE)   
        == 0 || DIE("Couldn't init SDL: " + SDL_GetError());
    SDL_SetVideoMode(800, 600, 32, SDL_OPENGL)
        || DIE("Couldn't set video mode: " + SDL_GetError());

    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 16, 0, 12, 50, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main() {
    std::srand(std::time(NULL));
    
    init_graphics();

    tvec ref = tvec(0, vec(8, 6));
    
    Artist* artist = new Artist;

    TerrainArray* block = new TerrainArray(vec(0,0), 16, 12);
    MessageList* messages = new MessageList;
    Board* board = new Board;
    for (int i = 0; i < 200; i++) {
        board->add_actor(new Actor(new PikemanState(tvec(0, vec(randrange(0,16),randrange(0,12))), vec(0,0)), messages->tracker()));
    }

    int frames = 0;
    
    while (1) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                SDL_Quit();
                return 0;
            }

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
                messages->send(new Messages::ChangeDirection(ref, vec(-2,0)));
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
                messages->send(new Messages::ChangeDirection(ref, vec(2,0)));
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
                messages->send(new Messages::ChangeDirection(ref, vec(0,2)));
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
                messages->send(new Messages::ChangeDirection(ref, vec(0,-2)));
            }
        }

        BoardView* view = new BoardView(board, ref);

        CLOCK += 0.003;
        ref.c += 0.003;
        board->step();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        artist->clear();
        block->draw(artist, ref);
        view->draw(artist);
        artist->render();
        SDL_GL_SwapBuffers();

        if (frames++ % 100 == 0) {
            std::cout << "t = " << ref.c << "\n";
        }
    }
}
