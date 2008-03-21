#ifndef __DUST_H__
#define __DUST_H__

#include <soy/config.h>
#include <soy/vec2.h>
#include <soy/util.h>
#include <list>

struct DustParticle {
	DustParticle(vec2 pos, num size) : pos(pos), size(size) { }
	vec2 pos;
	num size;
};

const int MAX_DUST_PARTICLES = 100;

class Dust {
public:
	Dust() { }

	void add_dust(vec2 pos) {
		if (rand() % 4 == 0) {
			particles_.push_back(DustParticle(pos + vec2(randrange(-1,1), randrange(-1,1)), 0.5));
		}
	}
	
	void step() {
		wind_accel_ += vec2(randrange(-1,1), randrange(-1,1));
		if (wind_accel_.norm2() > 5) {
			wind_accel_ = 5 * ~wind_accel_;
		}
		if (rand() % 200 == 0) {
			wind_accel_ += 30*vec2(randrange(-1,1), randrange(-1,1));
		}
		wind_ += DT * wind_accel_;
		if (wind_.norm() > 1) {
			wind_ = ~wind_;
		}
		
		for (std::list<DustParticle>::iterator i = particles_.begin(); 
				i != particles_.end();) {
			i->pos += DT * (wind_ + 0.6*vec2(randrange(-1,1), randrange(-1,1)));
			i->size += 0.5 * DT;

			if (int(rand() % 250) < int(particles_.size() - MAX_DUST_PARTICLES)
					|| i->size > 6) {
				std::list<DustParticle>::iterator j = i;
				++j;
				particles_.erase(i);
				i = j;
			}
			else {
				++i;
			}
		}
	}

	void draw() const {
		static const int sides = 8;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (std::list<DustParticle>::const_iterator i = particles_.begin(); 
				i != particles_.end(); ++i) {
			glPushMatrix();
			glTranslatef(i->pos.x, i->pos.y, 0);
			glBegin(GL_TRIANGLE_FAN);
			glColor4f(0.73, 0.48, 0.33, 1.0/(i->size+1));
			glVertex2f(0,0);
			glColor4f(0.73, 0.48, 0.33, 0);
			for (int s = 0; s <= sides; s++) {
				num theta = 2*M_PI*s / sides;
				glVertex2f(i->size*cos(theta), i->size*sin(theta));
			}
			glEnd();
			glPopMatrix();
		}	
		glDisable(GL_BLEND);
	}
private:
	std::list<DustParticle> particles_;
	vec2 wind_, wind_accel_;
};

#endif
