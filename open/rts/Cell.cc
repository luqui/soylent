#include "Cell.h"
#include "UnitSpeedAptitude.h"
#include "Board.h"
#include <GL/gl.h>
#include <GL/glu.h>

num Cell::get_angle_brightness(vec2 pos) const {
	num h = height();
	Cell* cellx = BOARD.get_cell(pos + vec2(1,0));
	Cell* celly = BOARD.get_cell(pos + vec2(0,1));
	num hx = cellx ? cellx->height() : h;
	num hy = celly ? celly->height() : h;
	vec2 norm(2/M_PI * atan(hx - h), 2/M_PI * atan(hy - h));
	return 1 - norm.norm();
}

void DirtCell::draw(vec2 pos) const {
	num b = get_angle_brightness(pos);
	glColor3f(b*0.63, b*0.38, b*0.23);
	draw_square(pos, pos + vec2(1,1));
}

num DirtCell::speed_aptitude(const UnitSpeedAptitude& apt) const {
	return apt.apt(TERRAIN_DIRT);
}


void ShallowWaterCell::draw(vec2 pos) const {
	num b = get_angle_brightness(pos);
	glColor3f(b*0.30, b*0.55, b*1);
	draw_square(pos, pos + vec2(1,1));
}

num ShallowWaterCell::speed_aptitude(const UnitSpeedAptitude& apt) const {
	return apt.apt(TERRAIN_SHALLOW_WATER);
}


void DeepWaterCell::draw(vec2 pos) const {
	num b = get_angle_brightness(pos);
	glColor3f(b*0.15, b*0.35, b*0.7);
	draw_square(pos, pos + vec2(1,1));
}

num DeepWaterCell::speed_aptitude(const UnitSpeedAptitude& apt) const {
	return apt.apt(TERRAIN_DEEP_WATER);
}



void RoadCell::draw(vec2 pos) const {
	num b = get_angle_brightness(pos);
	glColor3f(b*0.6, b*0.55, b*0.5);
	draw_square(pos, pos + vec2(1,1));
}

num RoadCell::speed_aptitude(const UnitSpeedAptitude& apt) const {
	return apt.apt(TERRAIN_ROAD);
}
