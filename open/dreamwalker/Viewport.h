#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "config.h"
#include "soy/vec2.h"
#include <GL/gl.h>
#include <GL/glu.h>

class Viewport {
public:
	Viewport(vec2 center, vec2 dim) : center(center), dim(dim) { }

	void make_active() const {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(center.x - dim.x/2, center.x + dim.x/2,
				   center.y - dim.y/2, center.y + dim.y/2);
		glMatrixMode(GL_MODELVIEW);
	}

	vec2 mouse_to_world(vec2 mouse) const {
		return vec2(mouse.x * dim.x / INIT.width()  + center.x - dim.x/2,
				    (INIT.height() - mouse.y) * dim.y / INIT.height() + center.y - dim.y/2);
	}

	vec2 world_to_mouse(vec2 world) const {
		return vec2((world.x - center.x + dim.x/2) * INIT.width()  / dim.x,
				    (dim.y/2 - world.y + center.y) * INIT.height() / dim.y);
	}
	
	vec2 center, dim;
};

#endif
