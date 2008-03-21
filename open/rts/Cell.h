#ifndef __CELL_H__
#define __CELL_H__

#include "config.h"
#include <soy/drawing.h>

enum TerrainType {
	TERRAIN_DIRT,
	TERRAIN_SHALLOW_WATER,
	TERRAIN_DEEP_WATER,
	TERRAIN_ROAD
};

class UnitSpeedAptitude;

class Cell {
	friend class Unit;
public:
	Cell() : unit_head_(0) { }
	virtual ~Cell() { }

	virtual void draw(vec2 pos) const = 0;
	virtual num speed_aptitude(const UnitSpeedAptitude& apt) const = 0;
	virtual num height() const = 0;  // This represents the height in
									 // the lower-left corner of the cell
	num get_angle_brightness(vec2 pos) const;
private:
	Unit* unit_head_;
};

class DirtCell : public Cell {
public:
	DirtCell(num height) : height_(height) { }
	void draw(vec2 pos) const;
	num speed_aptitude(const UnitSpeedAptitude& apt) const;
	num height() const { return height_; }
private:
	num height_;
};

class ShallowWaterCell : public Cell {
public:
	ShallowWaterCell(num height) : height_(height) { }
	void draw(vec2 pos) const;
	num speed_aptitude(const UnitSpeedAptitude& apt) const;
	num height() const { return height_; }
private:
	num height_;
};

class DeepWaterCell : public Cell {
public:
	DeepWaterCell(num height) : height_(height) { }
	void draw(vec2 pos) const;
	num speed_aptitude(const UnitSpeedAptitude& apt) const;
	num height() const { return height_; }
private:
	num height_;
};

class RoadCell : public Cell {
public:
	RoadCell(num height) : height_(height) { }
	void draw(vec2 pos) const;
	num speed_aptitude(const UnitSpeedAptitude& apt) const;
	num height() const { return height_; }
private:
	num height_;
};

#endif
