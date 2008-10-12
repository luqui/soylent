#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <iostream>

#include "chipmunk.h"

//An entity is an object with a shape and body which exists in a space. It applies friction to itself.
class Entity
{
public:
	Entity(cpSpace *space, cpBody *body, cpShape *shape, cpFloat radius = 1) 
	{
		myRadius = radius;
		myBody = body;
		myBody->data = this;
		cpSpaceAddBody(space, myBody);
		cpSpaceAddShape(space, shape);


		thrust = 900.0f;
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
	cpFloat myRadius;

	void ApplyFriction()
	{
		cpVect vel = myBody->v;
		cpVect dir = cpvnormalize(cpvneg(vel));
		cpFloat mag = cpvlength(vel) * uFrict; 
		
		if(mag != 0) Impulse(cpvmult(dir, mag));

		//Apply rotational friction too
		mag = myBody->w * uFrict / 2;
		if(mag != 0) 
		{
			cpBodyApplyImpulse(myBody, cpv(0, -mag), cpv(myRadius, 0));
			cpBodyApplyImpulse(myBody, cpv(0, mag), cpv(-myRadius, 0));
		}
	}

	void Impulse(cpVect force)
	{
		cpBodyApplyImpulse(myBody, force, cpvzero);
	}
};

#endif