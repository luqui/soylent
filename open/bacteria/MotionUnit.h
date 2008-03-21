#ifndef __MOTIONUNIT_H__
#define __MOTIONUNIT_H__

#include "BUnit.h"
#include "BInterp.h"
#include "Cell.h"
#include <soy/util.h>
#include <cmath>

class MotionUnit : public BUnit {
public:
    MotionUnit() : BUnit(1,1), dtheta_(0) { }

    void receive(BInterp* interp, int input, energy_t particle) {
        if (particle == ENERGY_LOCOMOTION) {
            interp->cell()->add_impulse(direction_);
            interp->cell()->rotate(dtheta_);
        }
        else {
            send(interp, 0, particle);
        }
    }

    void randomize() {
        double magnitude = randrange(0, MAX_VELOCITY_PER_ENERGY);
        double theta = randrange(0, 2 * M_PI);
        direction_ = vec2(magnitude * cos(theta), magnitude * sin(theta));
        dtheta_ = randrange(-0.1, 0.1);
    }

    void mutate() {
        if (randrange(0,1) < MOTION_MUTATION_RATE) {
            direction_ += MOTION_MUTATION * vec2(randrange(-1, 1), randrange(-1, 1));
            if (direction_.norm2() > MAX_VELOCITY_PER_ENERGY) {
                direction_ = MAX_VELOCITY_PER_ENERGY * ~direction_;
            }
        }
        if (randrange(0,1) < MOTION_ANGLE_MUTATION_RATE) {
            dtheta_ += MOTION_ANGLE_MUTATION * randrange(-1,1);
        }
    }

    void die(BInterp* interp) { }

    void step(BInterp* interp) { }

    MotionUnit* clone() const {
        MotionUnit* r = new MotionUnit;
        copy_into(r);
        r->direction_ = direction_;
        r->dtheta_ = dtheta_;
        return r;
    }

    double size() const {
        return 1;
    }

    Color color() const {
        return Color(1,0.5,0);
    }

private:
    vec2 direction_;
    double dtheta_;
};

#endif
