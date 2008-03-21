#include "BInterp.h"
#include "Cell.h"

void BInterp::release(energy_t particle) {
    cell_->eject(particle);
}
