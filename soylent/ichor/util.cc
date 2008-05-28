#include "util.h"
#include "FluidGrid.h"

void add_wide_velocity(FluidGrid* field, vec pos, vec vel) {
	field->add_velocity(pos, vel);
	field->add_velocity(pos + vec(1, 0), vel);
	field->add_velocity(pos + vec(-1,0), vel);
	field->add_velocity(pos + vec(0, 1), vel);
	field->add_velocity(pos + vec(0,-1), vel);
}
