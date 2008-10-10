#ifndef __AVATAR_H__
#define __AVATAR_H__

#include <assert.h>
#include <iostream>

#include "SDL.h"
#include "chipmunk.h"
#include "input_manager.h"
#include "entity.h"

class Avatar : public Entity
{
public:
	Avatar(cpSpace *space, cpBody *body, cpShape *shape) 
		: Entity(space, body, shape)
	{
		thrust = 900.0f;
		uFrict = 0.03f;
	}
	~Avatar() {}

	void Thrust(cpVect dir)
	{
		//***this should be replaced with a more sophisticated function which just resets the thrust force
		cpBodyResetForces(myBody);

		thrustForce = cpvmult(cpvnormalize(dir), thrust);
		if(cpvlength(dir) != 0) {
			cpBodyApplyForce(myBody, thrustForce, cpvzero);
		}
	}

private:
	cpVect thrustForce;

};

//This class takes key bindings for up, down, left, and right.
//It executes it's Avatar's Thrust method when one of it's keys is pressed or released.
class Thrust_Command : public Input_Command
{
public:
	Thrust_Command(Avatar *theAvatar, SDL_Event *theEvents, int eventCount)
		: Input_Command(theEvents, eventCount)
	{
		assert(numEvents == 8);
		myAvatar = theAvatar;

		dir = cpvzero;
	}

	~Thrust_Command() { }

	void handle(SDL_Event *e)
	{
		bool keydown = (e->type == SDL_KEYDOWN);
		SDLKey key = e->key.keysym.sym;
		
		if(key == events[0].key.keysym.sym) {//up
			dir = keydown ? cpvadd(dir, cpv( 0,  1)) : cpvadd(dir, cpv( 0, -1));
		} 
		else if(key == events[1].key.keysym.sym) {//down
			dir = keydown ? cpvadd(dir, cpv( 0, -1)) : cpvadd(dir, cpv( 0,  1));
		} 
		else if(key == events[2].key.keysym.sym) {//left
			dir = keydown ? cpvadd(dir, cpv(-1,  0)) : cpvadd(dir, cpv( 1,  0));
		}
		else if(key == events[3].key.keysym.sym) {//right
			dir = keydown ? cpvadd(dir, cpv( 1,  0)) : cpvadd(dir, cpv(-1,  0));
		} 
		else std::cout<<"***ThrustCommand Error*** unhandled input!\n";
		
		myAvatar->Thrust(dir);
	}

private:
	Avatar *myAvatar;
	cpVect dir;
};

#endif
