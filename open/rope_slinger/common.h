#ifndef __COMMON_H__
#define __COMMON_H__

#include "ode/ode.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>

#include "tweak.h"

using namespace std;

typedef double num;

inline void quit() {
    SDL_Quit();
    exit(0);
}

inline int _die(const string& msg, char* file, int line) {
    cout << msg << " at " << file << " line " << line << endl;
    quit();
    return 0;
}

#define DIE(msg) _die(string() + msg, __FILE__, __LINE__)

inline num min(num x, num y) {
    return x <= y ? x : y;
}

inline num max(num x, num y) {
    return x >= y ? x : y;
}

// This mod is faster than fmod when |x/n| is close to 1.
// But more importantly, it takes the modulus rounding toward
// negative infinity, not toward zero like fmod. 
inline num smallmod(num x, num n) {
    while (x < 0) x += n;
    while (x >= n) x -= n;
    return x;
}

// Returns the smallest absolute difference between x and y mod n
inline num moddiff(num x, num y, num n) {
    x = smallmod(x,n);
    y = smallmod(y,n);
    return min(fabs(x - y),
           min(fabs(x - y + n),
               fabs(x - y - n)));
}

inline num sign(num x) {
    return x >= 0 ? 1 : -1;
}

extern int P1_SCORE;
extern int P2_SCORE;

#endif
