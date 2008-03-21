#include "Cell.h"
#include "ParticleField.h"

void Cell::eject_direction(energy_t particle, vec2 direction) {
    if (particle == ENERGY_FOOD
           && foodtime_ + FOODTIME_PER_ENERGY <= MAX_STORED_FOODTIME) {
        foodtime_ += FOODTIME_PER_ENERGY;
    }
    else {
        vec2 d = direction.rotate(theta_);
        if (d.norm2() > 0.001) {
            PARTICLES.put(pos_ + (radius_ + 0.1) * ~d, particle, vel_ + d);
        }
        else {
            eject(particle);  // pick another random direction and eject
        }
    }
}
