#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <list>

class Particle {
public:
	Particle(vec pos) : pos(pos) { }
	void step(FluidDensityGrid* field) {
		/* // TOROIDAL
		if (pos.x < 1) { pos.x = W - 2; }
		if (pos.x > W - 1) { pos.x = 1; }
		if (pos.y < 1) { pos.y = H - 2; }
		if (pos.y > H - 1) { pos.y = 1; } */

		vec grad(
			(fabs(field->get_density(pos + vec(1,0))) - fabs(field->get_density(pos + vec(-1,0)))) / 2.0,
			(fabs(field->get_density(pos + vec(0,1))) - fabs(field->get_density(pos + vec(0,-1)))) / 2.0);
		pos -= 0.001 * PARTICLEDRIFT * DT * ~grad;
	}

	vec pos;
};

void draw_particles(const std::list<Particle> particles) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(1.0);
	glColor4f(0.5, 1, 0.5, 0.25);
	glBegin(GL_POINTS);
	for (std::list<Particle>::const_iterator i = particles.begin(); i != particles.end(); ++i) {
		glVertex2f(i->pos.x, i->pos.y);
	}
	glEnd();
	glDisable(GL_BLEND);
}
		

#endif
