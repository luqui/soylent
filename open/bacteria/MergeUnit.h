#ifndef __MERGEUNIT_H__
#define __MERGEUNIT_H__

#include "BUnit.h"

class MergeUnit : public BUnit
{
public:
    // input:  0 = stream 0
    //         1 = stream 1
    // output: 0 = merged stream 0 and stream 1
    MergeUnit() : BUnit(2,1) { }

    void receive(BInterp* interp, int input, energy_t particle) {
        send(interp, 0, particle);
    }

    void randomize() { }

    void mutate() { }

    void die(BInterp* interp) { }

    void step(BInterp* interp) { }

    MergeUnit* clone() const {
        MergeUnit* r = new MergeUnit;
        copy_into(r);
        return r;
    }

    double size() const {
        return 0.5;
    }

    Color color() const {
        return Color(0.3,0.3,0.3);
    }
};

#endif
