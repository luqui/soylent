#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "physics.h"
#include "drawing.h"

const double PLAYER_RADIUS = 1;

class Player : public Body, public Geom
{
public:
	Player() : Geom(dCreateSphere(PHYS->space, PLAYER_RADIUS)), powerbounce_(false) {
		dGeomSetBody(geom, body);
		dGeomSetCategoryBits(geom, 1);
		dGeomSetCollideBits(geom, (unsigned long)(-1));
		dBodySetPosition(body, 5, 30, 0);
	}

	void step() {
		Uint8* keys = SDL_GetKeyState(0);	
		vec2 dir;
		double scale = 30;
		if (keys[SDLK_a]) {
			dir += vec2(-1, 0);
		}
		if (keys[SDLK_d]) {
			dir += vec2(1, 0);
		}
		if (dir.norm2() > 0) {
			dir = scale*~dir;
		}
		dBodyAddForce(body, dir.x, dir.y, 0);
	}

	void draw() const {
		glColor3f(1,1,1);
		draw_circle(body_pos(), PLAYER_RADIUS);
	}

	bool events(SDL_Event* e) {
		if(e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_s) {
			powerbounce_ = true;	
			return true;
		}
		if(e->type == SDL_KEYUP && e->key.keysym.sym == SDLK_s) {
			powerbounce_ = false;
			return true;
		}
		return false;
	}

	bool powerbounce() const { return powerbounce_; }

private:
	bool powerbounce_;
};

#endif
