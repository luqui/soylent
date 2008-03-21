#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "config.h"
#include "Cell.h"

class Level {
public:
	virtual ~Level() { }

	virtual void step() { }

	virtual CellLevel* cells() const = 0;
	virtual vec2 gravity() const { return vec2(0, -1); }
	virtual float jump_power() const { return 1; }
};

#endif
