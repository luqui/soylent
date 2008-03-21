#ifndef __VEC_H__
#define __VEC_H__

#include "common.h"

class vec {
public:
    vec() : x(0), y(0) { }
    vec(num x, num y) : x(x), y(y) { }
    
    vec& operator += (const vec& u) {
        x += u.x;
        y += u.y;
        return *this;
    }

    vec& operator -= (const vec& u) {
        return *this += -u;
    }

    vec& operator *= (num a) {
        x *= a;
        y *= a;
        return *this;
    }

    vec& operator /= (num a) {
        x /= a;
        y /= a;
        return *this;
    }

    vec operator - () const {
        return vec(-x, -y);
    }

    vec operator ~ () const {
        return vec(*this) /= norm();
    }

    num norm2() const {
        return x*x + y*y;
    }
    
    num norm() const {
        return sqrt(norm2());
    }
    
    num x;
    num y;
};

inline vec operator + (vec u, const vec& v) {
    return u += v;
}

inline vec operator - (vec u, const vec& v) {
    return u -= v;
}

inline vec operator * (vec u, num a) {
    return u *= a;
}

inline vec operator * (num a, vec u) {
    return u *= a;
}

inline vec operator / (vec u, num a) {
    return u /= a;
}

inline num operator * (const vec& u, const vec& v) {
    return u.x * v.x + u.y * v.y;
}

inline num operator % (const vec& u, const vec& v) {
    return u.x * v.y - u.y * v.x;
}
#endif
