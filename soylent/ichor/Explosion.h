#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__

#include <iostream>
#include "FluidGrid.h"
#include "color.h"

class Explosion {
public:
	virtual ~Explosion() { }

	virtual void draw() const = 0;
	virtual void step(FluidDensityGrid* field) = 0;
	virtual bool done() const = 0;
};

class InwardVortexExplosion : public Explosion
{
public:
	InwardVortexExplosion(Color color, vec pos, float power, float time, float radius = W) 
		: color_(color), pos_(pos), power_(power), timer_(0), time_(time), radius_(radius)
	{ }

	void step(FluidDensityGrid* field) {
		timer_ += DT;
		float radius = radius_ * timer_ / time_;
		float push = 50 * (radius > 5 ? 5 : radius);
		for (int i = 0; i < 40; i++) {
			float theta = 2 * M_PI * i / 40;
			vec p = radius*vec(cos(theta), sin(theta));
			vec splode = pos_ + p;
			if (splode.x > CLAMPW && splode.x < W-CLAMPW-1 && splode.y > CLAMPH & splode.y < H-CLAMPH-1) {
				field->add_velocity(splode, -push * ~(vec(-p.y,p.x)+p));
				field->add_density(splode, power_);
			}
		}
	}
	
	bool done() const {
		return timer_ >= time_;
	}

	void draw() const {
		glPushMatrix();
		glTranslatef(pos_.x, pos_.y, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float r1 = radius_ * timer_ / time_;
		float r0 = r1 - 30;
		float r2 = r1 + 1;
		if (r0 < 0) r0 = 0;

		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);
		for (int i = 0; i < 60; i++) {
			float theta0 = 2 * M_PI * i / 60;
			float theta1 = 2 * M_PI * (i+1) / 60;
			color_.set_color_a(0);
			glVertex2f(r0 * cos(theta0), r0 * sin(theta0));
			glVertex2f(r0 * cos(theta1), r0 * sin(theta1));
			color_.set_color_a(1 - timer_ / time_);
			glVertex2f(r1 * cos(theta1), r1 * sin(theta1));
			glVertex2f(r1 * cos(theta0), r1 * sin(theta0));
			color_.set_color_a(1 - timer_ / time_);
			glVertex2f(r1 * cos(theta0), r1 * sin(theta0));
			glVertex2f(r1 * cos(theta1), r1 * sin(theta1));
			color_.set_color_a(0);
			glVertex2f(r2 * cos(theta1), r2 * sin(theta1));
			glVertex2f(r2 * cos(theta0), r2 * sin(theta0));
		}
		glEnd();
		glDisable(GL_BLEND);
		glPopMatrix();
	}
private:
	const vec pos_;
	const float power_;
	float timer_;
	const float time_;
	const float radius_;
	const Color color_;
};

class DensityGlobExplosion : public Explosion
{
public:
	DensityGlobExplosion(vec pos, float time, float power, float radius, int steps = 1)
		: pos_(pos), time_(time), power_(power), radius_(radius), steps_(steps), timer_(0)
	{ }
	
	void step(FluidDensityGrid* field) {
		for (int i = 0; i < steps_; i++) {
			timer_ += DT / steps_;
			float radius = radius_ * timer_ / time_;
			put_circle(field, radius);
			if (radius / radius_ > 0.5) {
				put_circle(field, 1-radius);
			}
			if (0.7 < radius / radius_ && radius / radius_ < 1) {
				vec offs(randrange(-radius, radius), randrange(-radius, radius));
				field->add_density(pos_ + offs, power_);
			}
		}
	}

	void draw() const { }

	bool done() const {
		return timer_ >= time_;
	}
private:

	void put_circle(DensityGrid* field, float radius) {
		for (int i = 0; i < 80; i++) {
			float theta = 2 * M_PI * i / 80;
			vec p = radius*vec(cos(theta), sin(theta));
			vec splode = pos_ + p;
			if (splode.x > CLAMPW && splode.x < W-CLAMPW-1 && splode.y > CLAMPH & splode.y < H-CLAMPH-1) {
				field->add_density(splode, power_ / (80 * time_ * steps_));
			}
		}
	}
	
	const vec pos_;
	const float time_;
	const float power_;
	const float radius_;
	const int steps_;
	float timer_;
};

#endif
