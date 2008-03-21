#include <soy/init.h>
#include <soy/util.h>
#include <soy/Timer.h>
#include <soy/Viewport.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <list>
#include <iostream>

SoyInit INIT;

const double SCRLEFT = -16, SCRRIGHT = 16, SCRBOTTOM = -12, SCRTOP = 12;

const int FWIDTH  = 320;
const int FHEIGHT = 240;

const double XSTEP = (SCRRIGHT - SCRLEFT) / FWIDTH;
const double YSTEP = (SCRTOP - SCRBOTTOM) / FHEIGHT;

const double SMOOTHING = 0.005;

enum Contours {
    CONTOUR_FIRE,
    CONTOUR_LIFE,
    CONTOUR_PLASMA,
    NUM_CONTOURS
};

double CONTOUR[NUM_CONTOURS];

struct Cell {
    double value;
    bool crossing[NUM_CONTOURS];
};

Cell FIELD[FWIDTH][FHEIGHT];

struct Dude {
    double xpos, ypos;
    double color;
};

const double DUDE_MOTION = 0.2;
Dude DUDE;

const double VARFACTOR = 0.5;

const double BULLET_MOTION = 0.2;
struct Bullet {
    vec2 pos;
    vec2 vel;
    double color;
    double target;
    double life;
};

typedef std::list<Bullet> bullets_t;
bullets_t BULLETS;

void calc_crossing(int x, int y) {
    for (int c = 0; c < NUM_CONTOURS; c++) {
        FIELD[x][y].crossing[c] = false;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if ((FIELD[x][y].value - CONTOUR[c]) * (FIELD[x+dx][y+dy].value - CONTOUR[c]) < 0) {
                    FIELD[x][y].crossing[c] = true;
                    return;
                }
            }
        }
    }
}

void find_crossings() {
    for (int x = 1; x < FWIDTH-1; x++) {
        for (int y = 1; y < FHEIGHT-1; y++) {
            calc_crossing(x,y);
        }
    }
}

void set_field(int x, int y, double value) {
    if (isnan(FIELD[x][y].value)) {
        FIELD[x][y].value = value;
    }
}

void clear_field() {
    for (int i = 0; i < FWIDTH; i++) {
        for (int j = 0; j < FHEIGHT; j++) {
            FIELD[i][j].value = 0;
        }
    }
}

void init_field() {
    clear_field();
    find_crossings();
}

void color_from_density(const Cell& cell) {
    if (cell.crossing[CONTOUR_PLASMA]) {
        glColor3f(0.0, randrange(0.8, 1.0), randrange(0.9,1.0));
    }
    else if (cell.crossing[CONTOUR_FIRE]) {
        glColor3f(randrange(0.8,1.0),randrange(0,0.3),0);
    }
    else if (cell.crossing[CONTOUR_LIFE]) {
        glColor3f(randrange(0,0.2), randrange(0.8,1.0), randrange(0,0.2));
    }
    else {
        //glColor3f(0,0,0);
        double c = (cell.value + 10) / 40;
        glColor3f(c,c,c);
    }
}

void draw_field() {
    glBegin(GL_QUADS);
    double xp = SCRLEFT;
    for (int x = 0; x < FWIDTH-1; x++) {
        double yp = SCRBOTTOM;
        for (int y = 0; y < FHEIGHT-1; y++) {

            color_from_density(FIELD[x][y]);
            glVertex2f(xp,yp);
            color_from_density(FIELD[x+1][y]);
            glVertex2f(xp+XSTEP,yp);
            color_from_density(FIELD[x+1][y+1]);
            glVertex2f(xp+XSTEP,yp+YSTEP);
            color_from_density(FIELD[x][y+1]);
            glVertex2f(xp,yp+YSTEP);

            yp += YSTEP;
        }
        xp += XSTEP;
    }
    glEnd();
}

void draw_dude() {
    glPointSize(10.0);
    glColor3f(0,0.5,1);
    glBegin(GL_POINTS);
    glVertex2f(DUDE.xpos, DUDE.ypos);
    glEnd();
}

void draw()
{
    draw_field();
    draw_dude();
}

void gradient(double x, double y, double* ox, double *oy) {
    int xidx = int((x - SCRLEFT) / XSTEP);
    int yidx = int((y - SCRBOTTOM) / YSTEP);

    *ox = (FIELD[xidx+1][yidx].value - FIELD[xidx-1][yidx].value) / (2*XSTEP);
    *oy = (FIELD[xidx][yidx+1].value - FIELD[xidx][yidx-1].value) / (2*YSTEP);
}

void offsgradient(double offs, double x, double y, double* ox, double* oy) {
    int xidx = int((x - SCRLEFT) / XSTEP);
    int yidx = int((y - SCRBOTTOM) / YSTEP);

    *ox = ((FIELD[xidx+1][yidx].value - offs)*(FIELD[xidx+1][yidx].value - offs)
         - (FIELD[xidx-1][yidx].value - offs)*(FIELD[xidx-1][yidx].value - offs))
                 / (2*XSTEP);
    *oy = ((FIELD[xidx][yidx+1].value - offs)*(FIELD[xidx][yidx+1].value - offs)
         - (FIELD[xidx][yidx-1].value - offs)*(FIELD[xidx][yidx-1].value - offs)) 
             / (2*YSTEP);
}

void step_field() {
    for (int x = 0; x < FWIDTH-2; x += 2) {
        for (int y = 0; y < FHEIGHT-2; y += 2) {
            double avg = 0.25 * (FIELD[x][y].value + FIELD[x+1][y].value + FIELD[x][y+1].value + FIELD[x+1][y+1].value);
            FIELD[x][y].value     = ((1 - SMOOTHING) * FIELD[x][y].value + SMOOTHING * avg);
            FIELD[x+1][y].value   = ((1 - SMOOTHING) * FIELD[x+1][y].value + SMOOTHING * avg);
            FIELD[x][y+1].value   = ((1 - SMOOTHING) * FIELD[x][y+1].value + SMOOTHING * avg);
            FIELD[x+1][y+1].value = ((1 - SMOOTHING) * FIELD[x+1][y+1].value + SMOOTHING * avg);
        }
    }
    for (int x = 1; x < FWIDTH-3; x += 2) {
        for (int y = 1; y < FHEIGHT-3; y += 2) {
            double avg = 0.25 * (FIELD[x][y].value + FIELD[x+1][y].value + FIELD[x][y+1].value + FIELD[x+1][y+1].value);
            FIELD[x][y].value     = ((1 - SMOOTHING) * FIELD[x][y].value + SMOOTHING * avg);
            FIELD[x+1][y].value   = ((1 - SMOOTHING) * FIELD[x+1][y].value + SMOOTHING * avg);
            FIELD[x][y+1].value   = ((1 - SMOOTHING) * FIELD[x][y+1].value + SMOOTHING * avg);
            FIELD[x+1][y+1].value = ((1 - SMOOTHING) * FIELD[x+1][y+1].value + SMOOTHING * avg);
        }
    }
    for (int x = 1; x < FWIDTH-2; x++) {
        for (int y = 1; y < FHEIGHT-2; y++) {
            if (FIELD[x][y].crossing[CONTOUR_PLASMA]) {
                // pick a new destination
                FIELD[x+1][y].value = -20;
                FIELD[x][y+1].value = -20;
                FIELD[x-1][y].value = -20;
                FIELD[x][y-1].value = -20;
                int destx = x + rand()%3 - 1;
                int desty = y + rand()%3 - 1;
                FIELD[destx][desty].value = -FIELD[x][y].value;
                CONTOUR[CONTOUR_PLASMA] += 1e-5;
            }
            else if (FIELD[x][y].crossing[CONTOUR_FIRE]) {
                // pick a new destination
                int destx = x + rand()%3 - 1;
                int desty = y + rand()%3 - 1;
                FIELD[destx][desty].value = 0.5 * (FIELD[destx][desty].value + FIELD[x][y].value);
                CONTOUR[CONTOUR_FIRE] += 1e-5;
            }
            else if (FIELD[x][y].crossing[CONTOUR_LIFE]) {
                // pick a new destination
                int destx = x + rand()%3 - 1;
                int desty = y + rand()%3 - 1;
                std::swap(FIELD[x][y].value, FIELD[destx][desty].value);
                FIELD[x][y].value += 0.4;
                CONTOUR[CONTOUR_LIFE] += 1e-5;
                CONTOUR[CONTOUR_LIFE] -= 1e-6;
            }
        }
    }
}

void step_dude() {
    Uint8* keys = SDL_GetKeyState(NULL);
    if (keys[SDLK_a]) { DUDE.xpos -= DUDE_MOTION; }
    if (keys[SDLK_d]) { DUDE.xpos += DUDE_MOTION; }
    if (keys[SDLK_s]) { DUDE.ypos -= DUDE_MOTION; }
    if (keys[SDLK_w]) { DUDE.ypos += DUDE_MOTION; }
    DUDE.xpos = clamp(SCRLEFT, SCRRIGHT-XSTEP, DUDE.xpos);
    DUDE.ypos = clamp(SCRBOTTOM, SCRTOP-YSTEP, DUDE.ypos);
}

void step_bullets() {
    for (bullets_t::iterator i = BULLETS.begin(); i != BULLETS.end(); ) {
        if (i->life <= 0 || i->pos.x < SCRLEFT  || i->pos.x >= SCRRIGHT
                         || i->pos.y< SCRBOTTOM || i->pos.y >= SCRTOP) {
            BULLETS.erase(i++);
            continue;
        }

        int xidx = int((i->pos.x - SCRLEFT) / XSTEP);
        int yidx = int((i->pos.y - SCRBOTTOM) / YSTEP);

        double pvalue = FIELD[xidx][yidx].value;
        FIELD[xidx][yidx].value = i->color;
        i->life -= fabs(pvalue - i->color);
        
        double gradx, grady;
        offsgradient(i->target, i->pos.x, i->pos.y, &gradx, &grady);
        vec2 grad(gradx, grady);
        grad = ~grad;

        i->pos += i->vel;
        i->vel.x -= 0.0002 * gradx;
        i->vel.y -= 0.0002 * grady;
        i->vel = BULLET_MOTION * ~i->vel;
        
        ++i;
    }
}

void step()
{
    find_crossings();

    step_field();
    step_bullets();
    step_dude();

    for (int i = 0; i < 1; i++) {
        int xidx = rand() % FWIDTH;
        int yidx = rand() % FHEIGHT;
        FIELD[xidx][yidx].value += 16;
    }

    int mousex, mousey;
    Uint8 buts = SDL_GetMouseState(&mousex, &mousey);

    // firing
    if (buts & SDL_BUTTON(1)) {
        Viewport pix = INIT.pixel_view();
        Viewport screen(vec2((SCRRIGHT + SCRLEFT)/2, (SCRTOP + SCRBOTTOM)/2),
                         vec2(SCRRIGHT - SCRLEFT,     SCRTOP - SCRBOTTOM));
        vec2 pos = coord_convert(pix, screen, vec2(mousex, mousey));
        vec2 av(DUDE.xpos, DUDE.ypos);

        int xtidx = int((pos.x - SCRLEFT) / XSTEP);
        int ytidx = int((pos.x - SCRBOTTOM) / YSTEP);

        vec2 dir = ~(pos - av);

        Bullet b;
        b.pos = av + dir;
        b.vel = BULLET_MOTION * dir;
        b.color = CONTOUR[CONTOUR_FIRE] - 1;
        b.target = FIELD[xtidx][ytidx].value;
        b.life = 80;
        BULLETS.push_back(b);
    }
}

void init() {
    srand(time(NULL));
    INIT.set_fullscreen();
    INIT.init();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(SCRLEFT,SCRRIGHT,SCRBOTTOM,SCRTOP);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE
         || e.type == SDL_QUIT) {
            INIT.quit();
            exit(0);
        }
    }
}



int main() {
    CONTOUR[CONTOUR_FIRE] = -20;
    CONTOUR[CONTOUR_LIFE] = 20;
    CONTOUR[CONTOUR_PLASMA] = 40;
    init();
    init_field();

    FrameRateLockTimer timer(1/20.0);

    while (true) {
        timer.lock();
        events();
        step();

        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();
    }
}
