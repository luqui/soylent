#ifndef __AVATAR_H__
#define __AVATAR_H__

#include "chipmunk.h"

class Avatar
{
public:
	Avatar(cpSpace *space, cpBody *body, cpShape *shape) 
	{
		myBody = body;
		myBody->data = this;
		cpSpaceAddBody(space, myBody);
		cpSpaceAddShape(space, shape);

		thrust = 900.0;
		uFrict = 0.03;
	}
	~Avatar() {}

	void Update()
	{
		ApplyFriction();
	}

	void Thrust(cpVect dir, cpVect offset = cpv(0.0, 0.0))
	{
		cpBodyApplyForce(myBody, cpvmult(dir, thrust), offset);
	}

	void StopThrusting()
	{
		cpBodyResetForces(myBody);
	}

private:
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

	void Move(cpVect force, cpVect offset = cpv(0.0, 0.0))
	{
		cpBodyApplyImpulse(myBody, force, offset);
	}

};

#endif