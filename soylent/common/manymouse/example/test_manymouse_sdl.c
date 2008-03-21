/*
 * A test file for ManyMouse that visualizes input with the SDL library.
 *  Simple Directmedia Layer (SDL) can be found at http://libsdl.org/
 *
 * Please see the file LICENSE in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "manymouse.h"
#include "SDL.h"

#define MAX_MICE 128
#define SCROLLWHEEL_DISPLAY_TICKS 100

static int available_mice = 0;

typedef struct
{
    int connected;
    int x;
    int y;
    SDL_Color color;
    char name[64];
    Uint32 buttons;
    Uint32 scrolluptick;
    Uint32 scrolldowntick;
    Uint32 scrolllefttick;
    Uint32 scrollrighttick;
} Mouse;

static Mouse mice[MAX_MICE];


static void update_mice(int screen_w, int screen_h)
{
    ManyMouseEvent event;
    while (ManyMouse_PollEvent(&event))
    {
        Mouse *mouse;
        if (event.device >= (unsigned int) available_mice)
            continue;

        mouse = &mice[event.device];

        if (event.type == MANYMOUSE_EVENT_RELMOTION)
        {
            if (event.item == 0)
                mouse->x += event.value;
            else if (event.item == 1)
                mouse->y += event.value;
        }

        else if (event.type == MANYMOUSE_EVENT_ABSMOTION)
        {
            float val = (float) (event.value - event.minval);
            float maxval = (float) (event.maxval - event.minval);
            if (event.item == 0)
                mouse->x = (val / maxval) * screen_w;
            else if (event.item == 1)
                mouse->y = (val / maxval) * screen_h;
        }

        else if (event.type == MANYMOUSE_EVENT_BUTTON)
        {
            if (event.item < 32)
            {
                if (event.value)
                    mouse->buttons |= (1 << event.item);
                else
                    mouse->buttons &= ~(1 << event.item);
            }
        }

        else if (event.type == MANYMOUSE_EVENT_SCROLL)
        {
            if (event.item == 0)
            {
                if (event.value < 0)
                    mouse->scrolldowntick = SDL_GetTicks();
                else
                    mouse->scrolluptick = SDL_GetTicks();
            }
            else if (event.item == 1)
            {
                if (event.value < 0)
                    mouse->scrolllefttick = SDL_GetTicks();
                else
                    mouse->scrollrighttick = SDL_GetTicks();
            }
        }

        else if (event.type == MANYMOUSE_EVENT_DISCONNECT)
        {
            mice[event.device].connected = 0;
        }
    }
}


static void draw_mouse(SDL_Surface *screen, int idx)
{
    int i;
    Uint32 now = SDL_GetTicks();
    Mouse *mouse = &mice[idx];
    SDL_Rect r = { mouse->x, mouse->y, 10, 10 };
    Uint32 color = SDL_MapRGB(screen->format, mouse->color.r,
                              mouse->color.g, mouse->color.b);

    if (mouse->x < 0) mouse->x = 0;
    if (mouse->x >= screen->w) mouse->x = screen->w-1;
    if (mouse->y < 0) mouse->y = 0;
    if (mouse->y >= screen->h) mouse->y = screen->h-1;
    SDL_FillRect(screen, &r, color);  /* draw a square for mouse position. */

    /* now draw some buttons... */
    color = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
    for (i = 0; i < 32; i++)
    {
        if (mouse->buttons & (1 << i))  /* pressed? */
        {
            r.w = 20;
            r.x = i * r.w;
            r.h = 20;
            r.y = screen->h - ((idx+1) * r.h);
            SDL_FillRect(screen, &r, color);
        }
    }

    /* draw scroll wheels... */

    #define DRAW_SCROLLWHEEL(var, item) \
        if (var > 0) \
        { \
            if ((now - var) > SCROLLWHEEL_DISPLAY_TICKS) \
                var = 0; \
            else \
            { \
                r.w = r.h = 20; \
                r.y = idx * r.h; \
                r.x = item * r.w; \
                SDL_FillRect(screen, &r, color); \
            } \
        }

    DRAW_SCROLLWHEEL(mouse->scrolluptick, 0);
    DRAW_SCROLLWHEEL(mouse->scrolldowntick, 1);
    DRAW_SCROLLWHEEL(mouse->scrolllefttick, 2);
    DRAW_SCROLLWHEEL(mouse->scrollrighttick, 3);

    #undef DRAW_SCROLLWHEEL
}

static void draw_mice(SDL_Surface *screen)
{
    int i;
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    for (i = 0; i < available_mice; i++)
    {
        if (mice[i].connected)
            draw_mouse(screen, i);
    }
    SDL_Flip(screen);
}

static void initial_setup(int screen_w, int screen_h)
{
    int i;
    memset(mice, '\0', sizeof (mice));

    /* pick some random colors for each mouse. */
    for (i = 0; i < MAX_MICE; i++)
    {
        Mouse *mouse = &mice[i];
        mouse->x = screen_w / 2;
        mouse->y = screen_h / 2;
        mouse->color.r = (int) (255.0*rand()/(RAND_MAX+1.0));
        mouse->color.g = (int) (255.0*rand()/(RAND_MAX+1.0));
        mouse->color.b = (int) (255.0*rand()/(RAND_MAX+1.0));
    }
}


static void init_mice(void)
{
    available_mice = ManyMouse_Init();
    if (available_mice > MAX_MICE)
        available_mice = MAX_MICE;

    if (available_mice == 0)
        printf("No mice detected!\n");
    else
    {
        int i;
        for (i = 0; i < available_mice; i++)
        {
            const char *name = ManyMouse_DeviceName(i);
            strncpy(mice[i].name, name, sizeof (mice[i].name));
            mice[i].name[sizeof (mice[i].name) - 1] = '\0';
            mice[i].connected = 1;
            printf("#%d: %s\n", i, mice[i].name);
        }
    }
}


int main(int argc, char **argv)
{
    int must_quit = 0;
    int cursor = 0;
    SDL_Surface *screen;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed! %s\n", SDL_GetError());
        return 1;
    }

    screen = SDL_SetVideoMode(640, 480, 0, 0);
    if (screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed! %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_WM_SetCaption("Move your mice, R to rescan, G to (un)grab, S to show/hide, ESC to quit",
                        "manymouse");

    /*SDL_WM_GrabInput(SDL_GRAB_ON);*/

    SDL_ShowCursor(cursor);

    initial_setup(screen->w, screen->h);
    init_mice();

    while (!must_quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                must_quit = 1;
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    must_quit = 1;
                else if (e.key.keysym.sym == SDLK_g)
                {
                    SDL_GrabMode grab = SDL_WM_GrabInput(SDL_GRAB_QUERY);
                    grab = (grab == SDL_GRAB_ON) ? SDL_GRAB_OFF : SDL_GRAB_ON;
                    SDL_WM_GrabInput(grab);
                }
                else if (e.key.keysym.sym == SDLK_s)
                {
                    cursor = (cursor) ? 0 : 1;
                    SDL_ShowCursor(cursor);
                }
                else if (e.key.keysym.sym == SDLK_r)
                {
                    printf("\n\nRESCAN!\n\n");
                    ManyMouse_Quit();
                    init_mice();
                }
            }
        }
        update_mice(screen->w, screen->h);
        draw_mice(screen);
    }

    ManyMouse_Quit();
    SDL_Quit();
    return 0;
}

/* end of test_manymouse_sdl.c ... */

