#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <soy/util.h>
#include <GL/gl.h>

const int COMPONENTS = 2;

inline void hsv2rgb(double h, double s, double v, double* r, double* g, double* b) {
    // grey;
    if (s == 0) {
        *r = *g = *b = v;
        return;
    }

    h *= 3/M_PI;
    double i = floor(h);
    double f = h - i;
    double p = v * (1 - s);
    double q = v * (1 - s * f);
    double t = v * (1 - s * (1 - f));

    switch (int(i)) {
        case 0: *r = v;
                *g = t;
                *b = p;
                break;
        case 1: *r = q;
                *g = v;
                *b = p;
                break;
        case 2: *r = p;
                *g = v;
                *b = t;
                break;
        case 3: *r = p;
                *g = q;
                *b = v;
                break;
        case 4: *r = t;
                *g = p;
                *b = v;
                break;
        case 5:
        case 6: *r = v;
                *g = p;
                *b = q;
                break;
        default: abort();
                 break;
    }
}

struct Element {
    double component[COMPONENTS];

    Element() {
        for (int i = 0; i < COMPONENTS; i++) {
            component[i] = 0;
        }
    }

    double norm2() const {
        double sum2 = 0;
        for (int i = 0; i < COMPONENTS; i++) {
            sum2 += component[i] * component[i];
        }
        return sum2;
    }

    double norm() const {
        return sqrt(norm2());
    }

    void color(double* r, double* g, double* b, double* a) const {
        double theta = atan2(component[0],component[1]) + M_PI;
        double nor = norm();
        hsv2rgb(theta, nor > 2 ? 1/(nor-1) : 1, nor, r, g, b);
        *a = 0.5*(nor-1);
    }

    void set_color() const {
        double r,g,b,a;
        color(&r,&g,&b,&a);
        glColor4f(r,g,b,a);
    }

    void randomize() {
        for (int i = 0; i < COMPONENTS; i++) {
            component[i] = randrange(-1,1);
        }
    }

    Element& operator += (const Element& o) {
        for (int i = 0; i < COMPONENTS; i++) {
            component[i] += o.component[i];
        }
        return *this;
    }

    Element& operator -= (Element o) {
        return *this += (o *= -1);
    }

    Element operator - () const {
        Element r = *this;
        return r *= -1;
    }

    Element& operator *= (double o) {
        for (int i = 0; i < COMPONENTS; i++) {
            component[i] *= o;
        }
        return *this;
    }

    Element& operator /= (double o) {
        return *this *= 1/o;
    }
};

Element operator + (Element a, const Element& b) { return a += b; }

Element operator - (Element a, const Element& b) { return a -= b; }

Element operator * (double a, Element b) { return b *= a; }

Element operator * (Element a, double b) { return a *= b; }

Element operator / (Element a, double b) { return a /= b; }


#endif
