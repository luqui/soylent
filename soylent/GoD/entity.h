#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "chipmunk.h"

class Entity
{
public:
	Entity(cpSpace *space, cpBody *body, cpShape *shape) 
	{
		myBody = body;
		myBody->data = this;
		cpSpaceAddBody(space, myBody);
		cpSpaceAddShape(space, shape);

		thrust = 900.0;
		uFrict = 0.03f;
	}
	~Entity() {}

	virtual void Update()
	{
		ApplyFriction();
	}

protected:
	cpBody* myBody;
	cpFloat thrust;
	cpFloat uFrict;

	void ApplyFriction()
	{
		cpVect vel = myBody->v;
		cpVect dir = cpvnormalize(cpvneg(vel));
		cpFloat mag = cpvlength(vel) * uFrict; 
		
		if(mag != 0) Move(cpvmult(dir, mag));
	}

	void Move(cpVect force)
	{
		cpBodyApplyImpulse(myBody, force, cpvzero);
	}
};

#endif