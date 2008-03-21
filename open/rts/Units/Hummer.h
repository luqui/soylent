#ifndef __UNITS_HUMMER_H__
#define __UNITS_HUMMER_H__

#include "Unit.h"

class HummerUnit : virtual public VehicleUnit
{
public:
	HummerUnit(vec2 pos) : VehicleUnit(pos), tex_(load_texture("media/hummer.png")) {
		// 1 u/s = 5.45 mph
		apt_.set_apt(TERRAIN_DIRT, 6);
		apt_.set_apt(TERRAIN_SHALLOW_WATER, 1.5);
		apt_.set_apt(TERRAIN_ROAD, 12);

		// autos.com says the Hummer H1 accelerates to 60 mph (11 u/s)
		// in 9.4 seconds.  So, its acceleration is 1.17 u/s/s
		apt_.set_acceleration(1.17);
		// Breaks are strong.  We'll say they're about five times
		// as strong as the engine.
		apt_.set_deceleration(6);
		// Dunno about this one.
		apt_.set_turn_rate(3.0);
	}
	
	void draw() const {
		glColor3f(1,1,1);
		TextureBinding bind = tex_->bind_tex();
		glPushMatrix();
		glTranslatef(pos_.x, pos_.y, 0);
		glRotatef(180 / M_PI * theta_, 0, 0, 1);
		draw_textured_square(vec2(-1,-1), vec2(1,1));
		glPopMatrix();
	}
	
	bool intersects(vec2 in) const {
		// we'll consider the hummer a 1x1 block
		return inside_box(pos_ - vec2(0.5,0.5), pos_ + vec2(0.5,0.5), in);
	}

	BoundingBox get_bounding_box() const { 
		return BoundingBox(pos_ - vec2(0.5,0.5), pos_ + vec2(0.5,0.5));
	}
	
private:
	Texture* tex_;
};

#endif
