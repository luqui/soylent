#ifndef __VIEWBOX_H__
#define __VIEWBOX_H__

#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <utility>
#include <soy/vec2.h>
#include <soy/util.h>
#include <stack>

extern int W;
extern int H;
extern int XRES;
extern int YRES;

class ViewBox
{
public:
	const float width;
	const float height;
	const float left;
	const float bottom;

	ViewBox::ViewBox(float w, float h, float l, float b)
		:width(w), height(h), left(l), bottom(b){}
	
	void set_projection() const {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(left, left+width, bottom, bottom+height);
		glMatrixMode(GL_MODELVIEW);
	}

	vec2 mouse_to_world(vec2 mousePos) const {
		return vec2(mousePos.x * (width/XRES) + left, (YRES-mousePos.y) * (height/YRES) + bottom);
	}

	float point_size() const {
		float xs = ceil(float(XRES)/width);
		float ys = ceil(float(YRES)/height);
		return xs > ys ? xs - 1 : ys - 1;
	}
};

extern ViewBox* WORLDVIEW;
extern std::stack<ViewBox> VIEWSTACK;

void change_view(const ViewBox& newbox) {
	VIEWSTACK.pop();
	VIEWSTACK.push(newbox);
	VIEWSTACK.top().set_projection();
}

ViewBox new_box(const ViewBox& old, vec2 dim, vec2 fix) {
	float xrat = (fix.x - old.left) / old.width;
	float yrat = (fix.y - old.bottom) / old.height;

	return ViewBox(dim.x, dim.y, 
		clamp(double(0), W - dim.x, fix.x - xrat * dim.x), 
		clamp(double(0), H - dim.y, fix.y - yrat * dim.y));
}

#endif
