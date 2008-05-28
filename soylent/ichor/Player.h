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
	Player(Color color, int sign, vec pos, int life, Texture* tex) :
		color_(color), sign_(sign), pos_(pos), store_(0), 
		life_(life), tex_(tex) 
	{ }

	bool check_hit(DensityGrid* field_) {
		return field_->get_density(pos_) * sign_ < -DEATH_DENSITY;
	}

	void die() {
		life_--;
		store_ += DIE_ENERGY;
	}

	void step(FluidDensityGrid* field_) {
		float dfluid = EMPTY_RATE*store_ + INCOME_RATE;
		store_ -= EMPTY_RATE*store_*DT;
		field_->add_density(pos_, sign_*dfluid);
		
		add_wide_velocity(field_, pos_, blow_);

		pos_ += field_->get_velocity(pos_) * PLAYER_MOTION * DT;
		pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
		pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
	}

	void draw() const {
		{
			glColor3f(1,1,1);
			TextureBinding b = tex_->bind();
			draw_rect(pos_ - vec(3,3), pos_ + vec(3,3));
		}

		glPointSize(4.0);
		glEnable(GL_BLEND);
		glColor4f(1,1,1,0.5);
		glBegin(GL_POINTS);
			glVertex2f(pos_.x + 0.5*blow_.x, pos_.y + 0.5*blow_.y);
		glEnd();
		glDisable(GL_BLEND);
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

	vec position() const {
		return pos_;
	}

	void set_blow(vec blow) {
		blow_ = blow;
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

private:
	int sign_;
	vec pos_;
	float store_;
	int life_;
	Texture* tex_;
	vec blow_;
	Color color_;
};

#endif
