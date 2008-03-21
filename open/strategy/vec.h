#ifndef __VEC_H__
#define __VEC_H__

#include "vec.h"
#include <cmath>

class vec
{
public:
    vec(num x = 0, num y = 0) : x(x), y(y) { }

    num norm2() const {
        return x * x + y * y;
    }
    
    num norm() const {
        return float(sqrt(norm2()));
    }

    vec operator- () const {
        return vec(-x, -y);
    }

    vec operator~ () const {   // shorthand for normalized
        return vec(*this) /= norm();
    }

    vec& operator+= (const vec& v) {
        x += v.x; y += v.y;
        return *this;
    }

    vec& operator-= (const vec& v) {
        x -= v.x; y -= v.y;
        return *this;
    }

    vec& operator*= (num a) {
        x *= a; y *= a;
        return *this;
    }

    vec& operator/= (num a) {
        x /= a; y /= a;
        return *this;
    }

    num x, y;
};

inline vec operator+ (vec u, const vec& v) {
    return u += v;
}

inline vec operator- (vec u, const vec& v) {
    return u -= v;
}

// scalar multiplication
inline vec operator* (vec u, num a) {
    return u *= a;
}

inline vec operator* (num a, vec u) {
    return u *= a;
}

// scalar division

inline vec operator/ (vec u, num a) {
    return u /= a;
}

// dot product
inline num operator* (const vec& u, const vec& v) {
    return u.x * v.x + u.y * v.y;
}

// cross product
inline num operator% (const vec& u, const vec& v) {
    return u.x * v.y - u.y * v.x;
}


class tvec {   // minkowsky vector
public:
    tvec() : c(0), p() { }
    tvec(num c, const vec& p) : c(c), p(p) { }
    
    tvec& operator += (const tvec& v) { 
        c += v.c;  p += v.p; 
        return *this; 
    }

    tvec& operator -= (const tvec& v) {
        c -= v.c;  p -= v.p;
        return *this;
    };

    num norm2() const {
        return C*C*c*c - p.norm2();
    }
    
    num c;  vec p;
};

inline tvec operator + (tvec u, const tvec& v) {
    return u += v;
}

inline tvec operator - (tvec u, const tvec& v) {
    return u -= v;
}

#endif
