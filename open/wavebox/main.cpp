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
#include <cassert>

SoyInit INIT;

SDL_Joystick* joy = NULL;

std::map<int,int> AXIS_STATE;
std::map<int,bool> BUTTON_STATE;

Viewport VIEW = Viewport::from_bounds(vec2(-16,-12), vec2(16,12));

const double DT = 1/30.0;
const double DS = 1/44100.0;

SDL_AudioSpec* SPEC;

const double HALFSTEP = pow(2.0,1.0/12.0);
#define STEPS(n) pow(HALFSTEP,n)

double shape_tone(double t, double shape) {
    return 2*pow(0.5*(t+1), shape)-1;
}

struct Tone {
    double pitch;
    double volume;
    double shape;
    double modvol;
    double modfreq;

    double left;
    double right;
    double modleft;
    double modright;

    double fetch(double& t, double& mod) {
        t += pitch * DS;
        mod += modfreq * DS;
        double base = volume * shape_tone(sin(t), shape);
        base *= 1 + modvol * shape_tone(sin(mod), shape);
        return base;
    }

    double fetch_left() { return fetch(left,modleft); }
    double fetch_right() { return fetch(right,modright); }

    void sanitize1(double& t, double freq) {
        t = fmod(t,2*M_PI);
    }

    void sanitize() {
        sanitize1(left, pitch);
        sanitize1(right, pitch);
        sanitize1(modleft, modfreq);
        sanitize1(modright, modfreq);
    }
};

Tone TONE = {};


void audio_callback(void* udata, Uint8* instream, int len)
{
    Sint16* stream = (Sint16*)instream;
    Sint16* p = stream;

    while (len > 0) {
        *p++ = int(3000 * TONE.fetch_left());
        *p++ = int(3000 * TONE.fetch_right());

        len -= 4;
    }

    TONE.sanitize();
}

void init() {
    INIT.set_init_flags(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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

    SPEC = new SDL_AudioSpec;
    SPEC->freq = 44100;
    SPEC->format = AUDIO_S16SYS;
    SPEC->channels = 2;
    SPEC->samples = 1024;
    SPEC->callback = &audio_callback;
    SPEC->userdata = NULL;
    SDL_OpenAudio(SPEC, NULL) >= 0 || DIE("Couldn't open audio: " + SDL_GetError());
    SDL_PauseAudio(0);

    VIEW.activate();
}

void show_state() {
    for (std::map<int,int>::iterator i = AXIS_STATE.begin(); i != AXIS_STATE.end(); ++i) {
        std::cout << "Axis " << i->first << " = " << i->second << "\n";
    }
    for (std::map<int,bool>::iterator i = BUTTON_STATE.begin(); i != BUTTON_STATE.end(); ++i) {
        std::cout << "Button " << i->first << " = " << (i->second ? "down" : "up") << "\n";
    }
}

double astate(int anum);

double deadzonify(double in, double dead) {
    if (in > dead) {
        return (in - dead) / (1 - dead);
    }
    else if (in < -dead) {
        return (in + dead) / (1 + dead);
    }
    else {
        return 0;
    }
}

// Up Right Down Left - 12 13 14 15
// L trigger - 2
// L/R bumper - 6/7
// L analog - 0/1
// start - 8
// right analog - 4/3
// A,X - 0,4
class Input {
public:
    Input() 
        : sounding_(0)
        , quantize_(true)
        , lanalog_key_(0)
    { }

    void update() {
        quantize_ = !BUTTON_STATE[1]; // B
        double lanalog = get_lanalog_pitch();
        if (BUTTON_STATE[6]) { // L bumper
            sounding_ = lanalog;
        }
        else if (!BUTTON_STATE[7]) { // R bumper
            sounding_ = 0;
        }

        TONE.pitch = sounding_;
        TONE.volume = 1 - (astate(2)+1)/2;
        TONE.shape = 6 - 3*(1+astate(5));
        TONE.modvol = 0;
        TONE.modfreq = 0;
    }

    double get_lanalog_pitch() {
        double x = astate(0); // left analog
        double y = astate(1);
        if (x*x + y*y < 0.2) return 0;
        double steps = 12*(atan2(y,x)/(2*M_PI) + 0.5);
        if (quantize_) {
            steps = round(steps);
        }
        double mindist = HUGE_VAL;
        double absteps = 0;
        for (int i = -5; i < 5; i++) {
            double dist = fabs(12*i + steps - lanalog_key_);
            if (dist < mindist) {
                mindist = dist;
                absteps = 12*i + steps;
            }
        }
        lanalog_key_ = absteps;
        return 2*M_PI*440*STEPS(absteps);
    }
private:
    bool quantize_;
    double sounding_;
    double lanalog_key_;
};

Input INPUT;

void update() {
    INPUT.update();
}

void events() {
    SDL_Event e;
    static bool lastbutton = false;
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
            //update();
        }
        if (e.type == SDL_JOYBUTTONDOWN) {
            BUTTON_STATE[e.jbutton.button] = true;
            show_state();
            //update();
        }
        if (e.type == SDL_JOYBUTTONUP) {
            BUTTON_STATE[e.jbutton.button] = false;
            show_state();
            //update();
        }
    }
}

double astate(int anum) {
    return (AXIS_STATE[anum]) / 32768.0;
}

void step() {
    update();
}

void draw() {
}

int main() {
    srand(time(NULL));
    FrameRateLockTimer timer(DT);
    init();

    while (true) {
        timer.lock();
        events();
        step();

        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();
    }
}
