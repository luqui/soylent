#ifndef __AVATAR_H__
#define __AVATAR_H__

#include <assert.h>
#include <iostream>

#include "SDL.h"
#include "chipmunk.h"
#include "input_manager.h"

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
		uFrict = 0.03f;
	}
	~Avatar() {}

	void Update()
	{
		ApplyFriction();
	}

	void Thrust(cpVect dir)
	{
		cpBodyResetForces(myBody);
		if(cpvlength(dir) != 0) {
			cpBodyApplyForce(myBody, cpvmult(cpvnormalize(dir), thrust), cpvzero);
		}
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

	void Move(cpVect force)
	{
		cpBodyApplyImpulse(myBody, force, cpvzero);
	}

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
		bool keydown = e->type == SDL_KEYDOWN;
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