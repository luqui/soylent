#ifndef __EJECTORUNIT_H__
#define __EJECTORUNIT_H__

#include "BUnit.h"

class EjectorUnit : public BUnit
{
public:
    EjectorUnit() : BUnit(1,0) { }

    void receive(BInterp* interp, int input, energy_t particle) {
        interp->cell()->eject_direction(particle, direction_);
    }

    void randomize() {
        direction_ = random_unit_vector();
    }

    void mutate() {
        direction_ += EJECTOR_DIRECTION_MUTATION * random_unit_vector();
    }
    
    void die(BInterp* interp) { }

    void step(BInterp* interp) { }

    EjectorUnit* clone() const {
        EjectorUnit* e = new EjectorUnit;
        copy_into(e);
        e->direction_ = direction_;
        return e;
    }

    double size() const {
        return 1;
    }

    Color color() const {
        return Color(0.2,0.2,0.0);
    }

private:
    vec2 direction_;
};

#endif
