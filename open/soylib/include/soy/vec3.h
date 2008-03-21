#ifndef __SOYLIB_VEC3_H__
#define __SOYLIB_VEC3_H__

#include <cmath>

class vec3
{
public:
    vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) { }

    double norm2() const {
        return x * x + y * y + z * z;
    }
    
    double norm() const {
        return sqrt(norm2());
    }

    vec3 operator- () const {
        return vec3(-x, -y, -z);
    }

    vec3 operator~ () const {   // shorthand for normalized
        return vec3(*this) /= norm();
    }

    vec3& operator+= (const vec3& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    vec3& operator-= (const vec3& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }

    vec3& operator*= (double a) {
        x *= a; y *= a; z *= a;
        return *this;
    }

    vec3& operator/= (double a) {
        x /= a; y /= a; z /= a;
        return *this;
    }

    double x, y, z;
};

inline vec3 operator+ (vec3 u, const vec3& v) {
    return u += v;
}

inline vec3 operator- (vec3 u, const vec3& v) {
    return u -= v;
}

// scalar multiplication
inline vec3 operator* (vec3 u, double a) {
    return u *= a;
}

inline vec3 operator* (double a, vec3 u) {
    return u *= a;
}

// scalar division

inline vec3 operator/ (vec3 u, double a) {
    return u /= a;
}

// dot product
inline double operator* (const vec3& u, const vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// cross product
inline vec3 operator% (const vec3& u, const vec3& v) {
    return vec3(
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x);
}

#endif
