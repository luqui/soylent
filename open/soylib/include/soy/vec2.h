#ifndef __SOYLIB_VEC2_H__
#define __SOYLIB_VEC2_H__

#include <cmath>

class vec2
{
public:
    vec2(double x = 0, double y = 0) : x(x), y(y) { }

    double norm2() const {
        return x * x + y * y;
    }
    
    double norm() const {
        return sqrt(norm2());
    }

    vec2 operator- () const {
        return vec2(-x, -y);
    }

    vec2 operator~ () const {   // shorthand for normalized
        return vec2(*this) /= norm();
    }

    vec2& operator+= (const vec2& v) {
        x += v.x; y += v.y;
        return *this;
    }

    vec2& operator-= (const vec2& v) {
        x -= v.x; y -= v.y;
        return *this;
    }

    vec2& operator*= (double a) {
        x *= a; y *= a;
        return *this;
    }

    vec2& operator/= (double a) {
        x /= a; y /= a;
        return *this;
    }

    vec2 rotate(double a) const {
        double sina = sin(a), cosa = cos(a);
        return vec2(cosa * x - sina * y, sina * x + cosa * y);
    }

    double x, y;
};

inline vec2 operator+ (vec2 u, const vec2& v) {
    return u += v;
}

inline vec2 operator- (vec2 u, const vec2& v) {
    return u -= v;
}

// scalar multiplication
inline vec2 operator* (vec2 u, double a) {
    return u *= a;
}

inline vec2 operator* (double a, vec2 u) {
    return u *= a;
}

// scalar division

inline vec2 operator/ (vec2 u, double a) {
    return u /= a;
}

// dot product
inline double operator* (const vec2& u, const vec2& v) {
    return u.x * v.x + u.y * v.y;
}

// cross product
inline double operator% (const vec2& u, const vec2& v) {
    return u.x * v.y - u.y * v.x;
}

inline bool inside_box(vec2 ll, vec2 ur, vec2 p) {
	return ll.x <= p.x && p.x <= ur.x && ll.y <= p.y && p.y <= ur.y;
}

#endif
