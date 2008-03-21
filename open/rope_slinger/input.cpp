#include "input.h"
#include "level.h"

MouseResponder* MOUSE_FOCUS;

void events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_MOUSEMOTION:
                if (!MOUSE_FOCUS) break;
                MOUSE_FOCUS->mouse_motion_native(&e.motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (!MOUSE_FOCUS) break;
                MOUSE_FOCUS->mouse_button_native(&e.button);
                break;
            case SDL_QUIT:
                quit();
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    quit();
                break;
        }
    }
}

void MouseSelector::mouse_motion_native(SDL_MouseMotionEvent* e) {
    num xdiff = num(e->xrel) / num(PIXEL_WIDTH) 
                    * (LEVEL->right - LEVEL->left);
    num ydiff = num(e->yrel) / num(PIXEL_HEIGHT)
                    * (LEVEL->bottom - LEVEL->top);  // sic
    mouse_move(vec(xdiff, ydiff));
}

void MouseSelector::mouse_button_native(SDL_MouseButtonEvent* e) {
    switch (e->button) {
        case SDL_BUTTON_LEFT:
            if (e->state == SDL_PRESSED)  mouse_left_button_down();
            if (e->state == SDL_RELEASED) mouse_left_button_up();
            break;
        case SDL_BUTTON_MIDDLE:
            if (e->state == SDL_PRESSED)  mouse_middle_button_down();
            if (e->state == SDL_RELEASED) mouse_middle_button_up();
            break;
        case SDL_BUTTON_RIGHT:
            if (e->state == SDL_PRESSED)  mouse_right_button_down();
            if (e->state == SDL_RELEASED) mouse_right_button_up();
            break;
        case SDL_BUTTON_WHEELUP:
            if (e->state == SDL_PRESSED)  mouse_wheel(1);
            break;
        case SDL_BUTTON_WHEELDOWN:
            if (e->state == SDL_PRESSED)  mouse_wheel(-1);
            break;
    }
}
