#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Texture.h"
#include "FluidGrid.h"
#include "drawing.h"
#include "util.h"
#include "color.h"

class Player
{
public:
	virtual ~Player() {}
	virtual void step(GPUFluid* field) = 0;
	
	virtual void draw() const {
		{
			vec drawRad = vec(720/float(H), 720/float(H));
			glColor3f(1,1,1);
			TextureBinding b = tex_->bind();
			draw_rect(pos_ - drawRad, pos_ + drawRad);
		}
	}
	
	virtual vec position() const {
		return pos_;
	}

	virtual void set_blow(vec blow) {
		blow_ = blow;
	}

	virtual Color get_color() const {
		return color_;
	}

protected:
	Player(Color color, int sign, vec pos, Texture* tex) :
		 color_(color), sign_(sign), pos_(pos), tex_(tex)
	{ }

	int sign_;
	vec pos_;
	Texture* tex_;
	vec blow_;
	Color color_;
};

class Sprayer : public Player
{
public:
	Sprayer(vec pos, Texture* tex) : 
		Player(Color(1, 0, 0), 1, pos, tex)
	{ }

	void step(GPUFluid* field) {
		float dfluid = 20; //how much fluid I output per step

		pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
		pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);

		pos_ += move_ * PLAYER_MOTION * DT;

		if (blow_.norm2() > 0) {
			field->add_density(pos_, sign_*dfluid, 2.0/float(W));
			field->add_velocity(pos_, ~blow_*pow(blow_.norm(),3)/2000.0, 2.0/float(W));
		}
	}

	void set_move(vec move) {
		move_ = move;
	}

private:
	vec move_;
};

class Dueler : public Player
{
public:
	Dueler(Color color, int sign, vec pos, int life, Texture* tex) :
		Player(color, sign, pos, tex), store_(0), life_(life), braking_(false)
	{ }

	bool check_hit(GPUFluid* field_) {
		return field_->get_density(pos_) * sign_ < -DEATH_DENSITY;
	}

	void die() {
		life_--;
		store_ += DIE_ENERGY;
	}

	void step(GPUFluid* field) {
		float dfluid = EMPTY_RATE*store_ + INCOME_RATE;
		store_ -= EMPTY_RATE*store_*DT;

		if (!braking_) {
			pos_ += (field->get_velocity(pos_) + blow_) * PLAYER_MOTION * DT;
			field->add_velocity(pos_, blow_, 1.0/W);
			field->add_density(pos_, sign_*dfluid, 1.0/W);
		}
		else {
			field->add_velocity(pos_ + ~blow_, blow_, 1.0/W);
			field->add_density(pos_ + ~blow_, sign_*dfluid, 1.0/W);
		}
		
		pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
		pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
	}

	void draw_lives(vec ll, int grow) const {  // grow is a direction: 1 or -1
		color_.set_color();
		for (int i = 0; i < life_; i++) {
			draw_rect(ll + vec(grow*3*i,0), ll + vec(grow*(3*i+2), 2));
		}

		glLineWidth(3.0);
		glBegin(GL_LINES);
			glVertex2f(ll.x, ll.y);
			glVertex2f(ll.x+grow*store_, ll.y);
		glEnd();
	}

	int get_life() const {
		return life_;
	}

	void store(float amount) {
		store_ += amount;
	}

	Color get_color() const {
		return color_;
	}

	void extra_life() {
		life_++;
	}

	void set_brake(bool value) {
		braking_ = value;
	}

private:
	int life_;
	float store_;
	bool braking_;
};

#endif
