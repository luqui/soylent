#ifndef __REPRODUCTIONUNIT_H__
#define __REPRODUCTIONUNIT_H__

#include "BUnit.h"
#include <soy/util.h>

class ReproductionUnit : public BUnit
{
public:
    // inputs:  0 = resource input
    //          1 = top of brain of new organism
    // outputs: 0 = waste
    ReproductionUnit() 
        : BUnit(2,1) 
        , child_food_units_(0)
        , dtheta_(0)
        , accum_food_(0)
        , accum_mass_(0)
    { }

    void receive(BInterp* interp, int input, energy_t particle) {
        if (particle == ENERGY_FOOD && accum_food_ < child_food_units_) {
            accum_food_++;
        }
        else if (particle == ENERGY_MASS && accum_mass_ < interp->size()*interp->size()) {
            accum_mass_ += 1;
        }
        else {
            send(interp, 0, particle);
        }

        if (accum_food_ >= child_food_units_ && accum_mass_ >= interp->size()*interp->size()) {
            // we'll name our kid John
            Cell* johnny = interp->cell()->reproduce(
                    2 * interp->cell()->radius() * offset_,
                    interp->cell()->rotation() + dtheta_,
                    accum_food_ * FOODTIME_PER_ENERGY,
                    this->outputs_[0].first,
                    this->outputs_[0].second);
            if (randrange(0,1) < CELL_MUTATION_RATE) {
                johnny->mutate();
            }
            CELLS.push_back(johnny);

            accum_food_ -= child_food_units_;
            accum_mass_ -= interp->size()*interp->size();
        }
    }

    void randomize() {
        child_food_units_ += rand() % int(INITIAL_FOODTIME / FOODTIME_PER_ENERGY);
        if (child_food_units_ <= 0) { child_food_units_ = 1; }

        offset_ = random_unit_vector();
        dtheta_ = randrange(0, 2*M_PI);
    }

    void mutate() {
        if (randrange(0,1) < REPRODUCTION_FOOD_MUTATION_RATE) {
            child_food_units_ += rand() % 3 - 1;
        }
        if (randrange(0,1) < REPRODUCTION_DIRECTION_MUTATION_RATE) {
            offset_ += REPRODUCTION_DIRECTION_MUTATION * random_unit_vector();
            offset_ = ~offset_;
        }
        if (randrange(0,1) < REPRODUCTION_ANGLE_MUTATION_RATE) {
            dtheta_ += REPRODUCTION_ANGLE_MUTATION * randrange(-1,1);
        }
    }

    void die(BInterp* interp) {
        while (accum_food_ --> 0) {
            interp->release(ENERGY_FOOD);
        }
        while (accum_mass_ > 1) {
            accum_mass_ -= 1;
            interp->release(ENERGY_MASS);
        }
    }

    void step(BInterp* interp) { }

    double size() const {
        return 1;
    }

    ReproductionUnit* clone() const {
        ReproductionUnit* r = new ReproductionUnit;
        copy_into(r);
        r->child_food_units_ = child_food_units_;
        r->offset_ = offset_;
        r->dtheta_ = dtheta_;
        return r;
    }

    Color color() const {
        return Color(0.7,0.9,0.7);
    }

private:
    int child_food_units_;
    vec2 offset_;
    double dtheta_;

    int accum_food_;
    double accum_mass_;
};

#endif
