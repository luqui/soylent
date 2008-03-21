#ifndef __ENERGY_H__
#define __ENERGY_H__

// Energy - particles of energy

#include "tweak.h"
#include <soy/vec2.h>
#include <GL/gl.h>
#include <cassert>

typedef int energy_t;

enum {
    ENERGY_LOCOMOTION,
    ENERGY_MASS,
    ENERGY_FOOD,
    
    NUM_ENERGY_TYPES
};

class Particle {
public:
    Particle(vec2 pos, energy_t type, vec2 vel = vec2()) : pos_(pos), vel_(vel), type_(type)
    { }

    void step(bool* active) {
        if (vel_.norm2() > PARTICLE_FIELD_ACTIVE_THRESHOLD) { *active = true; }

        pos_ += DT * vel_;
        // damp a bit
        vel_ *= 1 - PARTICLE_DAMPING;
    }

    void draw_pt() const {
        switch (type_) {
            case ENERGY_LOCOMOTION: 
                glColor4f(1,0.6,0.1,0.4);
                break;
            case ENERGY_MASS:
                glColor4f(1,1,0.2,0.4);
                break;
            case ENERGY_FOOD:
                glColor4f(0.2, 0.8, 0.2,0.4);
                break;
            default:
                assert(0);
                break;
        }
        glVertex2f(pos_.x, pos_.y);
    }

    vec2 position() const { return pos_; }
    energy_t type() const { return type_; }

private:
    vec2 pos_;
    vec2 vel_;
    energy_t type_;
};

#endif
