#ifndef __UTILS_H__
#define __UTILS_H__

#include "config.h"

inline vec2 quantize(vec2 in)
{
	return vec2(int(in.x), int(in.y));
}

inline void draw_grid()
{
	const ViewBox& box = VIEWSTACK.top();

	double fade = (box.width - zoomWidth) / (W - zoomWidth);

	if (fade >= 0.5) return;   // we wouldn't draw anything anyway

	int left = box.left < 0 ? 0 : int(floor(box.left));
	int right = box.left+box.width > W ? W : int(ceil(box.left+box.width));
	int bottom = box.bottom < 0 ? 0 : int(floor(box.bottom));
	int top = box.bottom+box.height > H ? H : int(ceil(box.bottom+box.height));
    glLineWidth(1.0);
	glBegin(GL_LINES);
	for (int x = left; x < right+1; x++) {
		if (x % 5 == 0) glColor4f(0.1 - fade, 0.5 - fade, 0.1 - fade, ALPHA_FACTOR);
		else if (fade >= 0.3) continue;
		else glColor4f(0, 0.3 - fade, 0, ALPHA_FACTOR);
		glVertex2f(x, bottom);
		glVertex2f(x, top);
	}
	for (int y = bottom; y < top+1; y++) {
		if (y % 5 == 0) glColor4f(0.1 - fade, 0.5 - fade, 0.1 - fade, ALPHA_FACTOR);
		else if (fade >= 0.3) continue;
		else			glColor4f(0, 0.3 - fade, 0, ALPHA_FACTOR);
		glVertex2f(left, y);
		glVertex2f(right, y);
	}
	glEnd();
}

#endif
