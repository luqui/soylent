#include "soy/drawing.h"
#include <GL/gl.h>
#include <GL/glu.h>

void DLLEXPORT draw_square(vec2 ll, vec2 ur) {
	glBegin(GL_QUADS);
		glVertex2f(ll.x, ll.y);
		glVertex2f(ur.x, ll.y);
		glVertex2f(ur.x, ur.y);
		glVertex2f(ll.x, ur.y);
	glEnd();
}


void DLLEXPORT draw_textured_square(vec2 ll, vec2 ur) {
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);  glVertex2f(ll.x, ll.y);
		glTexCoord2f(1,1);  glVertex2f(ur.x, ll.y);
		glTexCoord2f(1,0);  glVertex2f(ur.x, ur.y);
		glTexCoord2f(0,0);  glVertex2f(ll.x, ur.y);
	glEnd();
}
