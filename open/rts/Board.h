#ifndef __BOARD_H__
#define __BOARD_H__

#include <vector>
#include <sstream>
#include <soy/error.h>
#include <soy/Viewport.h>
#include "Cell.h"

class Board
{
public:
	Board(int width, int height) 
		: width_(width), height_(height)
	{
		cells_.resize(width_);
		for (cells_t::iterator i = cells_.begin(); i != cells_.end(); ++i) {
			i->resize(height_);
		}
	}

	~Board() {
		for (int i = 0; i < width_; i++) {
			for (int j = 0; j < height_; j++) {
				delete cells_[i][j];
			}
		}
	}

	int width()  const { return width_; }
	int height() const { return height_; }

	void draw(const Viewport& viewport) const {
		int left   = int(floor(viewport.center.x - viewport.dim.x/2));
		int right  = int(ceil (viewport.center.x + viewport.dim.x/2))+1;
		int bottom = int(floor(viewport.center.y - viewport.dim.y/2));
		int top    = int(ceil (viewport.center.y + viewport.dim.y/2))+1;
		if (left < 0) left = 0;
		if (right > width_) right = width_;
		if (bottom < 0) bottom = 0;
		if (top > height_) top = height_;
		for (int i = left; i < right; i++) {
			for (int j = bottom; j < top; j++) {
				if (cells_[i][j]) {
					cells_[i][j]->draw(vec2(i,j));
				}
			}
		}
	}

	void set_cell(vec2 p, Cell* cell) {
		int x = int(p.x);  int y = int(p.y);
		if (0 <= x && x < width_ && 0 <= y && y < height_) {
			cells_[x][y] = cell;
		}
		else {
			std::stringstream ss;
			ss << "Can't set cell at position (" << x << "," << y 
			   << ") in board of size (" << width_ << "," << height_ << ")";
			DIE(ss.str());
		}
	}

	Cell* get_cell(vec2 p) const {
		int x = int(floor(p.x));  int y = int(floor(p.y));
		if (0 <= x && x < width_ && 0 <= y && y < height_) {
			return cells_[x][y];
		}
		else {
			return 0;
		}
	}

	num calculate_speed(vec2 p, const UnitSpeedAptitude& apt) {
		Cell* cell = get_cell(p);
		if (!cell) return 0;
		return cell->speed_aptitude(apt);
	}
private:
	const int width_, height_;
	
	typedef std::vector< std::vector< Cell* > > cells_t;
	cells_t cells_;
};

#endif
