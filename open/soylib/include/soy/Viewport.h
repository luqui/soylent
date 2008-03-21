#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "soy/vec2.h"
#include <GL/gl.h>
#include <GL/glu.h>

class Viewport
{
public:
    static Viewport from_center_dim(vec2 center, vec2 dim) {
        return Viewport(center, dim);
    }

    static Viewport from_bounds(vec2 ll, vec2 ur) {
        return Viewport(0.5 * (ll + ur), ur - ll);
    }

	void activate() const {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
        multiply();
		glMatrixMode(GL_MODELVIEW);
	}

    void multiply() const {
		gluOrtho2D(center_.x - dim_.x/2, center_.x + dim_.x/2, center_.y - dim_.y/2, center_.y + dim_.y/2);
    }

    bool inside(vec2 p) const {
        return p.x >= center_.x - dim_.x && p.x < center_.x + dim_.x
            && p.y >= center_.y - dim_.y && p.y < center_.y + dim_.y;
    }

    vec2 ll()     const { return center_ - dim_/2; }
    vec2 ur()     const { return center_ + dim_/2; }
    vec2 center() const { return center_; }
    vec2 dim()    const { return dim_; }

private:
	Viewport(vec2 center, vec2 dim) : center_(center), dim_(dim) { }

	vec2 center_, dim_;
};

inline vec2 coord_convert(const Viewport& from, const Viewport& to, vec2 p) {
	return vec2((p.x - from.center().x) * to.dim().x / from.dim().x + to.center().x,
			    (p.y - from.center().y) * to.dim().y / from.dim().y + to.center().y);
}

#endif
