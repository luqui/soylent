#ifndef __SOYLIB_UTIL_H__
#define __SOYLIB_UTIL_H__

#include <cstdlib>
#include <cmath>
#include "soy/Viewport.h"
#include "soy/vec2.h"

inline double randrange(double a, double b) {
	return double(rand()) / RAND_MAX * (b - a) + a;
}

template <class T>
inline T clamp(T min, T max, T in) {
    return in < min ? min
         : in > max ? max
         : in;
}

inline vec2 wrap_vector(const Viewport& view, vec2 v) {
    if      (v.x > view.ur().x) { v.x = view.ll().x; }
    else if (v.x < view.ll().x) { v.x = view.ur().x; }
    
    if      (v.y > view.ur().y) { v.y = view.ll().y; }
    else if (v.y < view.ll().y) { v.y = view.ur().y; }
    return v;
}

inline vec2 clamp_vector(const Viewport& view, vec2 v) {
    v.x = clamp(view.ll().x, view.ur().x, v.x);
    v.y = clamp(view.ll().y, view.ur().y, v.y);
    return v;
}

inline vec2 random_vector(const Viewport& v) {
    return vec2(randrange(v.ll().x, v.ur().x),
                randrange(v.ll().y, v.ur().y));
}

inline vec2 random_unit_vector() {
    double theta = randrange(0, 2*M_PI);
    return vec2(cos(theta), sin(theta));
}

#endif
