#include <soy/init.h>
#include <soy/util.h>
#include <soy/Timer.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <sstream>
#include <time.h>

SoyInit INIT;

const float DT = 1/20.0;

const int GRIDW = 240;
const int GRIDH = 180;
const int COEFS = 3;   // this has dependencies elsewhere in the code; change with care.
const int GRAN  = 64;  // the space of the program is cubic in this number.  128 will probably eat all your memory...
const float RDIFFUSE = 0.2;  // [0-1] amount that the rdiffuse function tranquilizes the simulation
const float RDIFFUSE_PROB = 0.01; // [0-1] probability per frame that such a tranquilization will happen
const float RMUTATE = 1.5;  // [0-1] strength with which new curves are added to the state space
const float RMUTATE_PROB = DT/2.5;  // [0-1] probability pre frame that a new curve will be added


struct Cell {
    float value[COEFS];
};

struct Diffuse {
    float sum;
    float dir[4];
};

Diffuse DIFFUSE[COEFS];
typedef float react_t[GRAN][GRAN][GRAN][COEFS];
react_t REACT;

typedef Cell grid_t[GRIDW][GRIDH];
grid_t  GRID_FRONT;
grid_t  GRID_BACK;

void reset_grid(grid_t* g) {
    memset(g, 0, sizeof(grid_t));
}

void init_react() {
    float gran1 = 1.0/GRAN;
    for (int a = 0; a < GRAN; a++) {
        for (int b = 0; b < GRAN; b++) {
            for (int c = 0; c < GRAN; c++) {
                REACT[a][b][c][0] = a * gran1;
                REACT[a][b][c][1] = b * gran1;
                REACT[a][b][c][2] = c * gran1;
            }
        }
    }
}

void init_diffuse() {
    for (int i = 0; i < COEFS; i++) {
        float sum = 0;
        for (int j = 0; j < 4; j++) {
            DIFFUSE[i].dir[j] = randrange(0,1);
            sum += DIFFUSE[i].dir[j];
        }
        DIFFUSE[i].sum = sum;
    }
}

void randomize_field() {
    int ca = (rand() % 5) * (rand() % 4);
    int cb = (rand() % 5) * (rand() % 4);
    int cc = (rand() % 5) * (rand() % 4);
    int cd = (rand() % 5) * (rand() % 4);
    int ce = (rand() % 5) * (rand() % 4);
    int cf = (rand() % 5) * (rand() % 4);
    int cg = (rand() % 5) * (rand() % 4);

    int count = 0;
    for (int i = 1; i < GRIDW-1; i++) {
        for (int j = 1; j < GRIDH-1; j++) {
            for (int k = 0; k < COEFS; k++) {
                GRID_FRONT[i][j].value[k] = (ca * i * i + cb * i + cc * j * j + cd * j + ce * k * k + cf * k + count * cg) % 71 / 71.0;
                count++;
            }
        }
    }
}

void init() {
    INIT.init();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,GRIDW,0,GRIDH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    reset_grid(&GRID_FRONT);
    reset_grid(&GRID_BACK);

    init_react();
    init_diffuse();

    randomize_field();
}

#define GRANCLAMP(x) \
    if (x < 0) x = 0; \
    if (x >= GRAN) x = GRAN-1;

void step() {
    reset_grid(&GRID_BACK);

    // diffuse
    for (int i = 1; i < GRIDW-1; i++) {
        for (int j = 1; j < GRIDH-1; j++) {
            for (int k = 0; k < COEFS; k++) {
                GRID_BACK[i][j].value[k] += GRID_FRONT[i][j].value[k] * (1 - DT * DIFFUSE[k].sum);
                GRID_BACK[i+1][j].value[k] += DT * DIFFUSE[k].dir[0] * GRID_FRONT[i][j].value[k];
                GRID_BACK[i-1][j].value[k] += DT * DIFFUSE[k].dir[1] * GRID_FRONT[i][j].value[k];
                GRID_BACK[i][j+1].value[k] += DT * DIFFUSE[k].dir[2] * GRID_FRONT[i][j].value[k];
                GRID_BACK[i][j-1].value[k] += DT * DIFFUSE[k].dir[3] * GRID_FRONT[i][j].value[k];
            }
        }
    }

    for (int i = 0; i < GRIDW; i++) {
        for (int k = 0; k < COEFS; k++) {
            GRID_BACK[i][0].value[k] = 1;
            GRID_BACK[i][GRIDH-1].value[k] = 1;
        }
    }
    
    for (int j = 0; j < GRIDH; j++) {
        for (int k = 0; k < COEFS; k++) {
            GRID_BACK[0][j].value[k] = 1;
            GRID_BACK[GRIDW-1][j].value[k] = 1;
        }
    }

    for (int i = 1; i < GRIDW-1; i++) {
        for (int j = 1; j < GRIDH-1; j++) {
            int a = int(GRID_BACK[i][j].value[0] * GRAN + 0.5);
            GRANCLAMP(a);
            int b = int(GRID_BACK[i][j].value[1] * GRAN + 0.5);
            GRANCLAMP(b);
            int c = int(GRID_BACK[i][j].value[2] * GRAN + 0.5);
            GRANCLAMP(c);
            for (int k = 0; k < COEFS; k++) {
                GRID_FRONT[i][j].value[k] = REACT[a][b][c][k];
            }
        }
    }
}

void set_color(int x, int y) {
    glColor3f(GRID_FRONT[x][y].value[0], GRID_FRONT[x][y].value[1], GRID_FRONT[x][y].value[2]);
}

void draw() {
    glBegin(GL_QUADS);
    for (int x = 0; x < GRIDW-1; x++) {
        for (int y = 0; y < GRIDH-1; y++) {
            set_color(x,y);      glVertex2i(x,y);
            set_color(x+1,y);    glVertex2i(x+1,y);
            set_color(x+1,y+1);  glVertex2i(x+1,y+1);
            set_color(x,y+1);    glVertex2i(x,y+1);
        }
    }
    glEnd();
}

void modify_reaction(int xidx, int yidx) {

    int a = int(GRID_FRONT[xidx][yidx].value[0] * GRAN + 0.5);
    int aw = rand() % 25;
    int b = int(GRID_FRONT[xidx][yidx].value[1] * GRAN + 0.5);
    int bw = rand() % 25;
    int c = int(GRID_FRONT[xidx][yidx].value[2] * GRAN + 0.5);
    int cw = rand() % 25;
    int coef = rand() % COEFS;

    float value = randrange(0,1);

    for (int i = a-aw; i < a+aw; i++) {
        if (i < 0 || i >= GRAN) continue;
        for (int j = b-bw; j < b+bw; j++) {
            if (j < 0 || j >= GRAN) continue;
            for (int k = c-cw; k < c+cw; k++) {
                if (k < 0 || k >= GRAN) continue;
                
                float ad = fabs(float(i-a))/aw;
                float bd = fabs(float(j-b))/bw;
                float cd = fabs(float(k-c))/cw;

                float blend = RMUTATE * exp(-ad*ad - bd*bd - cd*cd);
                REACT[i][j][k][coef] = blend * value + (1-blend) * REACT[i][j][k][coef];
            }
        }
    }


}

void step_reaction() {
    Uint8* keys = SDL_GetKeyState(NULL);

    if (!keys[SDLK_SPACE] && randrange(0,1) < RMUTATE_PROB) {
		// introduce spike
		int xidx = rand() % GRIDW;
		int yidx = rand() % GRIDH;
		modify_reaction(xidx,yidx);
    }

    // diffuse toward identity
    float gran1 = 1.0/GRAN;
    if (randrange(0,1) < RDIFFUSE_PROB) {
        std::cout << "RDIFFUSE\n";
        for (int i = 0; i < GRAN; i++) {
            for (int j = 0; j < GRAN; j++) {
                for (int k = 0; k < GRAN; k++) {
                    REACT[i][j][k][0] = RDIFFUSE * i * gran1 + (1 - RDIFFUSE) * REACT[i][j][k][0];
                    REACT[i][j][k][1] = RDIFFUSE * j * gran1 + (1 - RDIFFUSE) * REACT[i][j][k][1];
                    REACT[i][j][k][2] = RDIFFUSE * k * gran1 + (1 - RDIFFUSE) * REACT[i][j][k][2];
                }
            }
        }
    }
}

void step_diffusion() {
    for (int k = 0; k < COEFS; k++) {
        float sum = 0;
        for (int d = 0; d < 4; d++) {
            DIFFUSE[k].dir[d] += DT * randrange(-1,1);
            if (DIFFUSE[k].dir[d] < 0) DIFFUSE[k].dir[d] = 0;
            sum += DIFFUSE[k].dir[d];
        }

        if (DT * sum > 1) {
            float norm = 1 / (DT * sum);
            sum = 0;
            for (int d = 0; d < 4; d++) {
                DIFFUSE[k].dir[d] *= norm;
                sum += DIFFUSE[k].dir[d];
            }
        }
        DIFFUSE[k].sum = sum;
    }
}

void save_state() {
    int ct = 0;
    bool exists = true;
    FILE* file;
    do {
        ct++;
        std::stringstream ss;
        ss << "dump_" << ct << ".rds";

        file = fopen(ss.str().c_str(), "r");
        if (file) fclose(file);
    }
    while (file);

    std::stringstream ss;
    ss << "dump_" << ct << ".rds";
    std::cout << "Dumping state to " << ss.str() << "\n";
    SDL_RWops* fp = SDL_RWFromFile(ss.str().c_str(), "w");

    SDL_RWwrite(fp, GRID_FRONT, sizeof(GRID_FRONT), 1);
    SDL_RWwrite(fp, DIFFUSE, sizeof(DIFFUSE), 1);
    SDL_RWwrite(fp, REACT,   sizeof(REACT),   1);

    SDL_RWclose(fp);
}

void load_state(std::string statefile) {
    SDL_RWops* fp = SDL_RWFromFile(statefile.c_str(), "r");
    if (!fp) {
        DIE("Couldn't open statefile");
    }

    SDL_RWread(fp, GRID_FRONT, sizeof(GRID_FRONT), 1);
    SDL_RWread(fp, DIFFUSE,    sizeof(DIFFUSE),    1);
    SDL_RWread(fp, REACT,      sizeof(REACT),      1);

    SDL_RWclose(fp);
}

void events() {
    SDL_Event e;

    Uint8* keys = SDL_GetKeyState(NULL);

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    INIT.quit();
                    exit(0);
                    break;
                case SDLK_TAB:
                    SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
                    break;
                case SDLK_f:
                    randomize_field();
                    break;
                case SDLK_t:
                    init_diffuse();
                    break;
                case SDLK_r:
                    init_react();
                    break;
                case SDLK_RETURN:
                    save_state();
                    break;
            }
        }

        if (e.type == SDL_MOUSEMOTION) {
            int xcoord = int(e.motion.x / INIT.pixel_view().dim().x * GRIDW);
            int ycoord = int((INIT.pixel_view().dim().y - e.motion.y) / INIT.pixel_view().dim().y * GRIDH);
            if (xcoord >= 0 && xcoord < GRIDW && ycoord >= 0 && ycoord < GRIDH) {
                Cell& cell = GRID_FRONT[xcoord][ycoord];
                if (keys[SDLK_a]) {
                    cell.value[0] += DT;
                    if (cell.value[0] > 1) cell.value[0] = 1;
                }
                if (keys[SDLK_s]) {
                    cell.value[1] += DT;
                    if (cell.value[1] > 1) cell.value[1] = 1;
                }
                if (keys[SDLK_d]) {
                    cell.value[2] += DT;
                    if (cell.value[2] > 1) cell.value[2] = 1;
                }

            }
        }

		if (e.type == SDL_MOUSEBUTTONDOWN) {
            int xcoord = int(e.button.x / INIT.pixel_view().dim().x * GRIDW);
            int ycoord = int((INIT.pixel_view().dim().y - e.button.y) / INIT.pixel_view().dim().y * GRIDH);
            if (xcoord >= 0 && xcoord < GRIDW && ycoord >= 0 && ycoord < GRIDH) {
				modify_reaction(xcoord, ycoord);
			}
		}
    }
}

int main(int argc, char** argv) {
    srand(unsigned(time(NULL)));
    init();

    if (argc == 2) {
        load_state(argv[1]);
    }

    FrameRateLockTimer timer(DT);

    while (true) {
        timer.lock();
        events();
        step();
        step_reaction();
        step_diffusion();

        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();
    }
}
