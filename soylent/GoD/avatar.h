#ifndef __AVATAR_H__
#define __AVATAR_H__

#include "chipmunk.h"

class Avatar
{
public:
	Avatar(cpSpace *space, cpBody *body, cpShape *shape) 
	{
		myBody = body;
		cpSpaceAddBody(space, body);
		cpSpaceAddShape(space, shape);
	}
	~Avatar() {}

#define MAG 10.0
	void MoveUp()
	{
		Move(cpv(0.0, MAG));
	}

	void MoveDown()
	{
		Move(cpv(0.0, -MAG));
	}

	void MoveLeft()
	{
		Move(cpv(-MAG, 0.0));
	}

	void MoveRight()
	{
		Move(cpv(MAG, 0.0));
	}


private:
	cpBody* myBody;

	void Move(cpVect force, cpVect offset = cpv(0.0, 0.0))
	{
		cpBodyApplyImpulse(myBody, force, offset);
	}

};

#endif