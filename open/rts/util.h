#ifndef __DRAWING_H__
#define __DRAWING_H__

#include <GL/gl.h>
#include <GL/glu.h>
#include <soy/vec2.h>
#include "config.h"
#include <soy/init.h>
#include <soy/Viewport.h>
#include <algorithm>

struct BoundingBox {
	BoundingBox(vec2 lli, vec2 uri) : ll(lli), ur(uri) { 
		if (ll.x > ur.x) std::swap(ll.x, ur.x);
		if (ll.y > ur.y) std::swap(ll.y, ur.y);
	}
	vec2 ll, ur;
};

inline bool intersects(vec2 pos, BoundingBox box) {
	return box.ll.x <= pos.x && pos.x <= box.ur.x
	    && box.ll.y <= pos.y && pos.y <= box.ur.y;
}

inline bool intersects(BoundingBox a, BoundingBox b, bool flip = true) {
	return intersects(a.ll, b)
		|| intersects(a.ur, b)
		|| intersects(vec2(a.ll.x, a.ur.y), b)
		|| intersects(vec2(a.ur.x, a.ll.y), b)
		|| flip && intersects(b, a, false);
}

inline void draw_selection_box(BoundingBox box) {
	const num bevel = 0.2;
	box.ll -= vec2(0.2, 0.2);
	box.ur += vec2(0.2, 0.2);

	glLineWidth(1.0);
	glBegin(GL_LINES);
		glVertex2f(box.ll.x, box.ll.y);
		glVertex2f(box.ll.x + bevel, box.ll.y);
		glVertex2f(box.ll.x, box.ll.y);
		glVertex2f(box.ll.x, box.ll.y + bevel);

		glVertex2f(box.ur.x, box.ll.y);
		glVertex2f(box.ur.x - bevel, box.ll.y);
		glVertex2f(box.ur.x, box.ll.y);
		glVertex2f(box.ur.x, box.ll.y + bevel);
		
		glVertex2f(box.ur.x, box.ur.y);
		glVertex2f(box.ur.x - bevel, box.ur.y);
		glVertex2f(box.ur.x, box.ur.y);
		glVertex2f(box.ur.x, box.ur.y - bevel);
		
		glVertex2f(box.ll.x, box.ur.y);
		glVertex2f(box.ll.x + bevel, box.ur.y);
		glVertex2f(box.ll.x, box.ur.y);
		glVertex2f(box.ll.x, box.ur.y - bevel);
	glEnd();
}

inline vec2 mouse_pos(const Viewport& viewport, int mx, int my) {
	return coord_convert(INIT.pixel_view(), viewport, vec2(mx, my));
}

inline vec2 mouse_pos(const Viewport& viewport) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	return mouse_pos(viewport, mx, my);
}

#endif
