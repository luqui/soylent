#ifndef __DRAWING_H__
#define __DRAWING_H__

#include <GL/gl.h>
#include <soy/vec2.h>

struct Color
{
	Color(double r, double g, double b, double a = 1) : r(r), g(g), b(b), a(a) { }
	
	void set() const {
		glColor4d(r,g,b,a);
	}

	double r, g, b, a;
};

void draw_box(vec2 ll, vec2 ur) {
	glBegin(GL_QUADS);
		glVertex2f(ll.x, ll.y);
		glVertex2f(ll.x, ur.y);
		glVertex2f(ur.x, ur.y);
		glVertex2f(ur.x, ll.y);
	glEnd();
}

void draw_circle(vec2 center, double radius, int sides = 24) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(center.x, center.y);
	for (int i = 0; i <= sides; i++) {
		double theta = 2 * M_PI * i / 24;
		glVertex2f(center.x + radius*cos(theta), center.y + radius*sin(theta));
	}
	glEnd();
}

#endif
