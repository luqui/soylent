#include "BUnit.h"
#include "BInterp.h"

#include "MotionUnit.h"
#include "ReproductionUnit.h"
#include "EjectorUnit.h"
#include "RationUnit.h"
#include "ConverterUnit.h"
#include "MergeUnit.h"

BUnit* make_random_BUnit() {
    BUnit* r = 0;
    switch (rand() % 5) {
        case 0: r = new MotionUnit; break;
        case 1: r = new ReproductionUnit; break;
        case 2: r = new EjectorUnit; break;
        case 3: r = new RationUnit; break;
        case 4: r = new ConverterUnit; break;
        case 5: r = new MergeUnit; break;
    }

    r->randomize();
    return r;
}

void BUnit::send(BInterp* interp, int output, energy_t particle) {
    interp->send(outputs_[output].first, outputs_[output].second, particle);
}
