#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <soy/vec2.h>
#include "physics.h"
#include "Metronome.h"
#include "Sound.h"

class Platform
{
public:
	virtual ~Platform() { }

	virtual double clipping_radius() const = 0;
	virtual void draw() const = 0;
	virtual void step() { }
	virtual bool collide() = 0;  // returns true if the ball should bounce off it
	virtual SoundEvent* sound() = 0;
};

class FixedPlatform : public Platform, public Geom
{
public:
	FixedPlatform(Technique* tech, vec2 pos, vec2 dim)
		: Geom(dCreateBox(PHYS->space, dim.x, dim.y, 1)),
		  tech_(tech)
	{
		dGeomSetPosition(geom, pos.x, pos.y, 0);
		dGeomSetCategoryBits(geom, 2);
		dGeomSetCollideBits(geom, 1);

		int scale[8] = { 0, 2, 3, 5, 6, 7, 9, 10 };
		note_ = 60 + 12*int(pos.y/16) + scale[int(pos.y/2)%8] - 12;
		dur_  = dim.x/8;
	}

	void draw() const 
	{
		dVector3 dim;
		const dReal* pos = dGeomGetPosition(geom);
		dGeomBoxGetLengths(geom, dim);
		
		glColor3f(0.7, 0.7, 0.7);
		draw_box(vec2(pos[0] - dim[0]/2, pos[1] - dim[1]/2),
				 vec2(pos[0] + dim[0]/2, pos[1] + dim[1]/2));
	}

	double clipping_radius() const {
		dVector3 dims;
		dGeomBoxGetLengths(geom, dims);
		return sqrt(dims[0]*dims[0] + dims[1]*dims[1]);
	}

	bool collide() {
		return true;
	}

	SoundEvent* sound() {
		return new SoundEvent(tech_, 
				note_, int(randrange(64,96)),
				dur_);
	}
protected:
	Technique* tech_;
	int note_;
	double dur_;
};

class RegenPlatform : public FixedPlatform
{
public:
	RegenPlatform(Technique* tech, Metronome* metro, const Beat& beat, vec2 center, vec2 dim) : 
		FixedPlatform(tech, center, dim), metro_(metro), beat_(beat), activated_(true)
	{ }

	void step() {
		if (metro_->quantum(beat_)) { activated_ = true; }
	}
	
	void draw() const {
		dVector3 dim;
		const dReal* pos = dGeomGetPosition(geom);
		dGeomBoxGetLengths(geom, dim);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		if (activated_) {
			glColor4f(0.7, 0.7, 0.7, 1);
		}
		else {
			glColor4f(0.7, 0.7, 0.7, 0.4);
		}
		draw_box(vec2(pos[0] - dim[0]/2, pos[1] - dim[1]/2),
				 vec2(pos[0] + dim[0]/2, pos[1] + dim[1]/2));
		
		glDisable(GL_BLEND);
	}

	bool collide() {
		bool ret = activated_;
		activated_ = false;
		return ret;
	}
protected:
	Metronome* metro_;
	Beat beat_;
	bool activated_;
};


#endif
