#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <soy/Timer.h>
#include <vector>
#include <algorithm>

const float DT = 1/30.0;
const float FREQ = 44100;
void init();
void init_pattern();
void events();
void fillbuffer(void* userdata, Uint8* stream, int len);
void step();

struct WaveSpec {
    WaveSpec(float freq = 0, float phase = 0, float amp = 0, int channel = 0)
        : freq(freq), phase(phase), amp(amp), channel(channel)
    { }
    
    float freq;
    float phase;
    float amp;
    int channel;
};
typedef std::vector<WaveSpec> pattern_t;
pattern_t PATTERN;

enum {
    BETAL,
    BETAR,
    ALPHAL,
    ALPHAR,
    THETAL,
    THETAR,
    DELTAL,
    DELTAR,
    N_WAVES
};


int main(int argc, char** argv) {
    FrameRateLockTimer timer(DT);
    
    init();
    init_pattern();
    
    while (true) {
        timer.lock();
        events();
        step();
    }
}

void step() {
    static float t = 0;
    t += DT;
}

void init_pattern() {
    PATTERN.resize(N_WAVES);
    PATTERN[BETAL]  = WaveSpec(19.621/2, 0, 0, 0);
    PATTERN[BETAR]  = WaveSpec(19.621/2, 0, 0, 1);
    PATTERN[ALPHAL] = WaveSpec(10.000/2, 0, 0, 0);
    PATTERN[ALPHAR] = WaveSpec(10.000/2, 0, 0, 1);
    PATTERN[THETAL] = WaveSpec(5.1140/2, 0.5, 0, 0);
    PATTERN[THETAR] = WaveSpec(5.1140/2, 0, 0, 1);
    PATTERN[DELTAL] = WaveSpec(2.4170/2, 0, 0, 0);
    PATTERN[DELTAR] = WaveSpec(2.4170/2, 0, 0, 1);
}

void init() {
    std::srand(std::time(NULL));
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 1024;
    spec.callback = &fillbuffer;
    spec.userdata = NULL;
    if (SDL_OpenAudio(&spec, NULL) < 0) {
        DIE("Couldn't open audio driver: " + SDL_GetError());
    }

    if (SDL_SetVideoMode(1440, 900, 32, SDL_FULLSCREEN) == 0) {
        DIE("Couldn't open video driver: " + SDL_GetError());
    }

    SDL_PauseAudio(0);
}

void events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            SDL_Quit();
            std::exit(0);
        }
    }
}

void fillbuffer(void* userdata, Uint8* stream8, int len) {
    Sint16* stream = (Sint16*)stream8;
    len /= 4;  // 2 bytes per sample * 2 channels per sample
    static float t = 0;

    int nwaves = PATTERN.size();
    while (len --> 0) {
        int r = std::rand() % (1 << 14);
        stream[0] = stream[1] = 0;

        for (int i = 0; i < PATTERN.size(); i++) {
            WaveSpec& spec = PATTERN[i];
            stream[spec.channel] += Sint16(r * spec.amp * sin(M_PI * spec.freq * (t - spec.phase)));
        }
        stream += 2;
        t += 1/FREQ;
    }
}
