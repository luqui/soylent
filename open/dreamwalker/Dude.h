#ifndef __DUDE_H__
#define __DUDE_H__

#include "soy/vec2.h"
#include "soy/Texture.h"
#include "Cell.h"
#include "Level.h"
#include "config.h"

class Dude {
public:
	Dude() : texture_(load_texture("dude.png")) { }

	void draw() const {
		TextureBinding tex = texture_->bind();
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);  glVertex2f(pos_.x-0.5,   pos_.y-0.5);
			glTexCoord2f(0,0);  glVertex2f(pos_.x-0.5,   pos_.y+0.5);
			glTexCoord2f(1,0);  glVertex2f(pos_.x+0.5,   pos_.y+0.5);
			glTexCoord2f(1,1);  glVertex2f(pos_.x+0.5,   pos_.y-0.5);
		glEnd();
	}

	vec2 pos() const { return pos_; }
	vec2 vel() const { return vel_; }

	void set_pos(vec2 in) { pos_ = in; }
	void set_vel(vec2 in) { vel_ = in; }
	void add_force(vec2 in) { vel_ += DT * in; }

	void step() { 
		if (!FALLTHROUGH) {
			if (Cell* cell = LEVEL->cells()->get(int(floor(pos_.x)), int(floor(pos_.y)))) {
				// we're inside a block
				if (cell->concrete()) {
					// and we shouldn't be!
					pos_ = vec2(round(pos_.x), round(pos_.y));
				}
			}
			
			vec2 npos = pos_ + DT*vel_;
			if (Cell* cell = LEVEL->cells()->get(int(floor(npos.x)), int(floor(pos_.y)))) {
				cell->trigger();
				if (cell->concrete()) {
					vel_.x = -cell->elasticity() * vel_.x;
				}
			}
			if (Cell* cell = LEVEL->cells()->get(int(floor(pos_.x)), int(floor(npos.y)))) {
				cell->trigger();
				if (cell->concrete()) {
					vel_.y = -cell->elasticity() * vel_.y;
				}
			}
		}
		pos_ += DT*vel_;
	}
	
private:
	vec2 pos_;
	vec2 vel_;
	Texture* texture_;
};

#endif
