#ifndef __TERRAINGENERATOR_H__
#define __TERRAINGENERATOR_H__

#include "config.h"
#include "Board.h"

class TerrainGenerator {
public:
	TerrainGenerator() : board_(&BOARD) { }

	void set_board(Board* bd) { board_ = bd; }

	void generate();

private:
	void generate_height_map();
	void generate_height_squares(vec2 ll, vec2 ur);
	num height(vec2 pos);
	
	void place_cell(vec2 pos, Cell* cell);

	num avg_offset(num a, num b, num range) const;

	vec2 random_border_point();

	void generate_river(vec2 start, int amount, int width, int heigt);
	void generate_deep_water();
	bool is_water(vec2 pos);

	void generate_road(bool horiz, int width, int pos);
	
	Board* board_;
	std::vector< std::vector< num > > height_map_;
};

#endif
