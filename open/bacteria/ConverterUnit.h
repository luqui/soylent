#ifndef __CONVERTERUNIT_H__
#define __CONVERTERUNIT_H__

#include "BUnit.h"

class ConverterUnit : public BUnit
{
public:
    // input:  0 = input
    // output: 0 = converted
    //         1 = extra
    ConverterUnit() : BUnit(1,2), from_(0), to_(0) { }

    void receive(BInterp* interp, int input, energy_t particle) {
        if (particle == from_) {
            send(interp, 0, to_);
        }
        else {
            send(interp, 1, particle);
        }
    }

    void randomize() {
        from_ = rand() % NUM_ENERGY_TYPES;
        to_   = rand() % NUM_ENERGY_TYPES;
    }

    void mutate() {
        if (randrange(0,1) < 0.5) {
            from_ = rand() % NUM_ENERGY_TYPES;
        }
        else {
            to_ = rand() % NUM_ENERGY_TYPES;
        }
    }

    void die(BInterp* interp) { }

    void step(BInterp* interp) { }

    ConverterUnit* clone() const {
        ConverterUnit* r = new ConverterUnit;
        copy_into(r);
        r->from_ = from_;
        r->to_   = to_;
        return r;
    }

    double size() const {
        return 2;
    }

    Color color() const {
        return Color(0,0,1);
    }
private:
    energy_t from_;
    energy_t to_;
};

#endif
