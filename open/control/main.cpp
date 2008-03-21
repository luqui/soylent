#include <soy/init.h>
#include <soy/vec2.h>
#include <soy/Viewport.h>
#include <soy/Timer.h>
#include <soy/util.h>
#include <soy/Texture.h>
#include <iostream>
#include <map>
#include <vector>
#include <list>

SoyInit INIT;

SDL_Joystick* joy = NULL;

std::map<int,int> AXIS_STATE;
std::map<int,bool> BUTTON_STATE;

Viewport VIEW = Viewport::from_bounds(vec2(-16,-12), vec2(16,12));

enum Color { RED, GREEN, BLUE };

struct Avatar {
    Color color;
    vec2 pos;
    vec2 vel;
    double theta;
    double range;
    double radius;
};

const double DT = 1/60.0;
const double THRESH = 1.2;

int SCORE = 5;
bool FLASH = false;

std::vector<Avatar> AV;

struct Enemy {
    Color color;
    vec2 pos;
    double life;
};

std::list<Enemy> ENEMIES;

struct Powerup {
    Powerup() { 
        texture = 0; 
        pos = vec2(randrange(VIEW.ll().x + 3, VIEW.ur().x - 3)
                  ,randrange(VIEW.ll().y + 3, VIEW.ur().y - 3));
    }
    ~Powerup() { delete texture; }

    void draw() {
        TextureBinding b = texture->bind_tex();
        glBegin(GL_QUADS);
            glTexCoord2f(0,0);  glVertex2f(pos.x - 0.5, pos.y + 0.5);
            glTexCoord2f(1,0);  glVertex2f(pos.x + 0.5, pos.y + 0.5);
            glTexCoord2f(1,1);  glVertex2f(pos.x + 0.5, pos.y - 0.5);
            glTexCoord2f(0,1);  glVertex2f(pos.x - 0.5, pos.y - 0.5);
        glEnd();
    }
    virtual void step() = 0;
    virtual void collect(Avatar* collector) = 0;

    Texture* texture;
    vec2 pos;
};

struct PowerupRange : public Powerup {
    PowerupRange() {
        texture = load_texture("powerup_range.png");
    }
    void step() { }
    void collect(Avatar* av) {
        av->range *= M_SQRT2;
    }
};

struct PowerupRadius : public Powerup {
    PowerupRadius() {
        texture = load_texture("powerup_radius.png");
    }
    void step() { }
    void collect(Avatar* av) {
        av->radius /= M_SQRT2;
    }
};

struct PowerupLife : public Powerup {
    PowerupLife() {
        texture = load_texture("powerup_life.png");
    }
    void step() { }
    void collect(Avatar* av) {
        SCORE++;
    }
};

std::list<Powerup*> POWERUPS;

void init() {
    INIT.set_init_flags(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);
    INIT.init();

    int nj = SDL_NumJoysticks();
    if (nj > 0) {
        std::cout << "Opening joystick '" << SDL_JoystickName(0) << "'\n";
        joy = SDL_JoystickOpen(0);
    }
    else {
        std::cout << "No joysticks\n";
        INIT.quit();
        exit(0);
    }

    glEnable(GL_TEXTURE_2D);
    VIEW.activate();

    AV.resize(3);
    AV[0].color = RED;
    AV[0].theta = 0;
    AV[0].range = 1;  AV[0].radius = 1;
    AV[1].color = GREEN;
    AV[1].range = 1;  AV[1].radius = 1;
    AV[2].color = BLUE;
    AV[2].range = 1;  AV[2].radius = 1;
}

void show_state() {
    return;
    for (std::map<int,int>::iterator i = AXIS_STATE.begin(); i != AXIS_STATE.end(); ++i) {
        std::cout << "Axis " << i->first << " = " << i->second << "\n";
    }
    for (std::map<int,bool>::iterator i = BUTTON_STATE.begin(); i != BUTTON_STATE.end(); ++i) {
        std::cout << "Button " << i->first << " = " << (i->second ? "down" : "up") << "\n";
    }
}

void events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE
         || e.type == SDL_QUIT) {
            SDL_JoystickClose(joy);
            INIT.quit();
            exit(0);
        }

        if (e.type == SDL_JOYAXISMOTION) {
            AXIS_STATE[e.jaxis.axis] = e.jaxis.value;
            show_state();
        }
        if (e.type == SDL_JOYBUTTONDOWN) {
            BUTTON_STATE[e.jbutton.button] = true;
            show_state();
        }
        if (e.type == SDL_JOYBUTTONUP) {
            BUTTON_STATE[e.jbutton.button] = false;
            show_state();
        }
    }
}

void draw_circle(bool fill) {
    if (fill) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
    }
    else {
        glBegin(GL_LINE_LOOP);
    }

    for (int i = 0; i <= 24; i++) {
        double theta = 2 * M_PI * i / 24;
        glVertex2f(cos(theta), sin(theta));
    }
    glEnd();
}

void draw_avatar(Avatar* av) {
    glPushMatrix();
    glTranslatef(av->pos.x, av->pos.y, 0);
    glScalef(av->radius, av->radius, 1);
    draw_circle(true);
    glScalef(av->range / av->radius, av->range / av->radius, 1);
    draw_circle(false);
    glPopMatrix();
}

double astate(int anum) {
    int deadzone = 4000;
    if (AXIS_STATE[anum] < -deadzone) {
        return (AXIS_STATE[anum] + deadzone) / (32768.0 - deadzone);
    }
    else if (AXIS_STATE[anum] > deadzone) {
        return (AXIS_STATE[anum] - deadzone) / (32768.0 - deadzone);
    }
    else {
        return 0;
    }
}

void move_toward(Enemy* enemy, vec2 pos1, vec2 pos2) {
    if ((enemy->pos - pos1).norm2() < (enemy->pos - pos2).norm2()) {
        enemy->pos += DT * ~(pos1 - enemy->pos);
    }
    else {
        enemy->pos += DT * ~(pos2 - enemy->pos);
    }
}

void step_avatar(Avatar& av) {
    av.pos += DT * av.vel;
    av.vel *= std::pow(1 - 0.5, DT);
    if (av.pos.x + 1 > VIEW.ur().x) { av.vel.x = -fabs(av.vel.x); }
    if (av.pos.y + 1 > VIEW.ur().y) { av.vel.y = -fabs(av.vel.y); }
    if (av.pos.x - 1 < VIEW.ll().x) { av.vel.x =  fabs(av.vel.x); }
    if (av.pos.y - 1 < VIEW.ll().y) { av.vel.y =  fabs(av.vel.y); }
}

void step() {
    for (int j = 0; j < 3; j++) {
        for (std::list<Powerup*>::iterator i = POWERUPS.begin(); i != POWERUPS.end(); ++i) {
            if (((*i)->pos - AV[j].pos).norm2() <= AV[j].radius * AV[j].radius) {
                (*i)->collect(&AV[j]);
                delete *i;
                std::list<Powerup*>::iterator k = i++;
                POWERUPS.erase(k);
            }
        }
    }

    for (std::list<Enemy>::iterator i = ENEMIES.begin(); i != ENEMIES.end();) {
        bool del = false;

        if (i->color == RED)   { move_toward(&*i, AV[1].pos, AV[2].pos); }
        if (i->color == GREEN) { move_toward(&*i, AV[2].pos, AV[0].pos); }
        if (i->color == BLUE)  { move_toward(&*i, AV[0].pos, AV[1].pos); }

        for (int j = 0; j < 3; j++) {
            if ((AV[j].pos - i->pos).norm2() < AV[j].range * AV[j].range && AV[j].color == i->color) {
                del = true;
                break;
            }

            if ((AV[j].pos - i->pos).norm2() < AV[j].radius * AV[j].radius) {
                SCORE--;
                FLASH = true;
                del = true;
                break;

            }
        }
        
        i->life -= DT;
        if (i->life < 0) del = true;

        if (del) {
            std::list<Enemy>::iterator j = i++;
            ENEMIES.erase(j);
        }
        else {
            ++i;
        }
    }


    {
        double powLeft  = (AXIS_STATE[2] + 32768.0) / 65536.0;
        double powRight = (AXIS_STATE[5] + 32768.0) / 65536.0;
        AV[0].theta += 6 * DT * (powLeft - powRight);
        AV[0].vel += 6 * DT * std::min(powLeft, powRight) * vec2(cos(AV[0].theta), sin(AV[0].theta));
    }
    step_avatar(AV[0]);
    
    AV[1].vel += 6 * DT * vec2(astate(4), astate(3));
    step_avatar(AV[1]);
    AV[2].vel += 6 * DT * vec2(astate(0), -astate(1));
    step_avatar(AV[2]);


    static double enemytimer = 0;
    static double enemytime = 10;
    enemytime *= pow(1 - 0.01, DT);
    enemytimer -= DT;
    while (enemytimer <= 0) {
        enemytimer += enemytime;
        Enemy e;
        e.color = Color(rand() % 3);
        e.life = 30;
        double theta = randrange(0,2*M_PI);
        e.pos = 20 * vec2(cos(theta), sin(theta));
        ENEMIES.push_back(e);
    }

    static double poweruptimer = 60;
    poweruptimer -= DT;
    while (poweruptimer <= 0) {
        poweruptimer += 30;
        switch (rand() % 3) {
            case 0: POWERUPS.push_back(new PowerupRange); break;
            case 1: POWERUPS.push_back(new PowerupRadius); break;
            case 2: POWERUPS.push_back(new PowerupLife); break;
        }
    }
}

void draw() {
    {
        double left = VIEW.ll().x + 1;
        double top  = VIEW.ur().y - 1;
        for (int i = 0; i < SCORE; i++) {
            glColor3f(1,1,1);
            glBegin(GL_QUADS);
                glVertex2f(left, top);
                glVertex2f(left+0.3, top);
                glVertex2f(left+0.3, top-0.3);
                glVertex2f(left, top-0.3);
            glEnd();
            left += 0.6;
        }
    }

    glColor3f(1,1,1);
    for (std::list<Powerup*>::iterator i = POWERUPS.begin(); i != POWERUPS.end(); ++i) {
        (*i)->draw();
    }

    for (std::list<Enemy>::iterator i = ENEMIES.begin(); i != ENEMIES.end(); ++i) {
        switch (i->color) {
            case RED:   glColor3f(0.7,0,0);  break;
            case GREEN: glColor3f(0,0.7,0);  break;
            case BLUE:  glColor3f(0,0,0.7);  break;
        }
        glBegin(GL_TRIANGLES);
            glVertex2f(i->pos.x, i->pos.y + 0.3);
            glVertex2f(i->pos.x - 0.21, i->pos.y - 0.15);
            glVertex2f(i->pos.x + 0.21, i->pos.y - 0.15);
        glEnd();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glColor3f(0,1,0);
    draw_avatar(&AV[1]);
    glColor3f(0,0,1);
    draw_avatar(&AV[2]);
    
    glColor3f(1,0,0);
    draw_avatar(&AV[0]);
    glPushMatrix();
        glTranslatef(AV[0].pos.x, AV[0].pos.y, 0);
        glColor3f(1,1,1);
        glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(cos(AV[0].theta), sin(AV[0].theta));
        glEnd();
    glPopMatrix();
    glDisable(GL_BLEND);

    if (FLASH) {
        FLASH = false;
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0,0,0,1);
    }
}

int main() {
    srand(time(NULL));
    Timer gametime;
    FrameRateLockTimer timer(DT);
    init();

    gametime.init();
    while (SCORE > 0) {
        timer.lock();
        events();
        step();

        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();
    }
    std::cout << "You lasted " << gametime.get_time_diff() << " seconds!\n";
}
