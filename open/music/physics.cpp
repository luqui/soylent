#include "physics.h"

Physics* PHYS = 0;

void Physics::activate() {
	PHYS = this;
}
