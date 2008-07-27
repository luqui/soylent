#ifndef __AVATAR_H__
#define __AVATAR_H__

#include "chipmunk.h"

class Avatar
{
public:
	Avatar(cpSpace *space, cpBody *body, cpShape *shape) 
	{
		cpSpaceAddBody(space, body);
		cpSpaceAddShape(space, shape);
	}
	~Avatar() {}


private:

};

#endif