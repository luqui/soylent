#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <GL/gl.h>
#include <GL/glu.h>

struct Color {
	Color(float r, float g, float b) : r(r), g(g), b(b) { }
	
	void set() const {
		glColor3f(r,g,b);
	}
	
	float r, g, b;
};

#endif
