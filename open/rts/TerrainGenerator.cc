#include "TerrainGenerator.h"
#include "Cell.h"
#include <soy/util.h>

void TerrainGenerator::generate()
{
	generate_height_map();
	
	for (int x = 0; x < board_->width(); x++) {
		for (int y = 0; y < board_->height(); y++) {
			place_cell(vec2(x,y), new DirtCell(height(vec2(x,y))));
		}
	}
	
	int num_rivers = int(pow(double(2), randrange(-1, 3)));
	for (int i = 0; i < num_rivers; i++) {
		int amount = rand() % 1000 + 10;
		int heigt = rand() % 5 + 1;
		int width = rand() % 5 + 1;
		generate_river(random_border_point(), amount, width, heigt);
	}

	generate_deep_water();

	int num_roads = int(pow(double(2), randrange(-1, 3)));
	for (int i = 0; i < num_roads; i++) {
		int width = int(pow(double(2), randrange(0, 3)));
		generate_road(rand() % 2, width, rand() % (board_->width() * board_->height()));
	}
}

void TerrainGenerator::generate_height_map() {
	height_map_.resize(board_->width()+1);
	for (int i = 0; i < int(height_map_.size()); i++) {
		height_map_[i].resize(board_->height()+1);
	}
	num range = board_->width() * board_->height();
	height_map_[0][0]                = 0*randrange(-range, range)/2;
	height_map_[0][board_->width()]  = 0*randrange(-range, range)/2;
	height_map_[board_->height()][0] = 0*randrange(-range, range)/2;
	height_map_[board_->height()][board_->width()] = 0*randrange(-range, range)/2;
	generate_height_squares(vec2(0,0), vec2(board_->width(), board_->height()));
}

void TerrainGenerator::generate_height_squares(vec2 ll, vec2 ur) {
	// We are given the points on the square bounded by ll and ur,
	// and generate everything in between recursively
	
	num xrange = ur.x - ll.x;
	num yrange = ur.y - ll.y;
	if (xrange < 1 && yrange < 1) return;

	num range = xrange * yrange;
	const num variance = 0.003;
	// Generate the center point
	num center = 0.25 * (height_map_[int(ll.x)][int(ll.y)]
					   + height_map_[int(ll.x)][int(ur.y)]
					   + height_map_[int(ur.x)][int(ur.y)]
					   + height_map_[int(ur.x)][int(ll.y)]) 
				+ variance*randrange(-range, range)/2;
	
	// Generate the missing diamond
	num bottom = avg_offset(height_map_[int(ll.x)][int(ll.y)],
	                        height_map_[int(ur.x)][int(ll.y)], variance*xrange);
	num top    = avg_offset(height_map_[int(ll.x)][int(ur.y)],
	                        height_map_[int(ur.x)][int(ur.y)], variance*xrange);
	num left   = avg_offset(height_map_[int(ll.x)][int(ll.y)],
	                        height_map_[int(ll.x)][int(ur.y)], variance*yrange);
	num right  = avg_offset(height_map_[int(ur.x)][int(ll.y)],
	                        height_map_[int(ur.x)][int(ur.y)], variance*yrange);


	vec2 cp = 0.5*(ll + ur);
	
	height_map_[int(cp.x)][int(cp.y)] = center;
	height_map_[int(cp.x)][int(ll.y)] = bottom;
	height_map_[int(cp.x)][int(ur.y)] = top;
	height_map_[int(ll.x)][int(cp.y)] = left;
	height_map_[int(ur.x)][int(cp.y)] = right;
	
	// Generate the subsquares
	generate_height_squares(ll, cp);
	generate_height_squares(cp, ur);
	generate_height_squares(vec2(ll.x, cp.y), vec2(cp.x, ur.y));
	generate_height_squares(vec2(cp.x, ll.y), vec2(ur.x, cp.y));
}

num TerrainGenerator::height(vec2 pos) {
	if (int(pos.x) < 0 || int(pos.x) >= board_->width()+1
	 || int(pos.y) < 0 || int(pos.y) >= board_->height()+1) return 0;
	return height_map_[int(pos.x)][int(pos.y)];
}

void TerrainGenerator::place_cell(vec2 pos, Cell* cell) {
	if (int(pos.x) < 0 || int(pos.x) >= board_->width()
	 || int(pos.y) < 0 || int(pos.y) >= board_->height()) return;

	if (Cell* excell = board_->get_cell(pos)) {
		delete excell;
	}
	board_->set_cell(pos, cell);
}

vec2 TerrainGenerator::random_border_point() {
	switch (rand() % 4) {
		case 0:
			return vec2(0, randrange(0, board_->height()));
		case 1:
			return vec2(board_->width()-1, randrange(0, board_->height()));
		case 2:
			return vec2(randrange(0, board_->width()), 0);
		case 3:
			return vec2(randrange(0, board_->width()), board_->height()-1);
		default:  // Don't whine, warnings
			return vec2(); 
	}
}

num TerrainGenerator::avg_offset(num a, num b, num range) const {
	return 0.5 * (a + b) + randrange(-range, range)/2;
}

void TerrainGenerator::generate_river(vec2 start, int amount, int width, int heigt) {
	vec2 cur = start;
	while (amount --> 0) {
		for (int i = 0; i < heigt; i++) {
			for (int j = 0; j < width; j++) {
				place_cell(cur + vec2(i,j), new ShallowWaterCell(height(cur + vec2(i,j))));
			}
		}

		Cell* right = board_->get_cell(cur + vec2( 1, 0));
		Cell* left  = board_->get_cell(cur + vec2(-1, 0));
		Cell* up    = board_->get_cell(cur + vec2( 0, 1));
		Cell* down  = board_->get_cell(cur + vec2( 0,-1));

		num min_height = HUGE_VAL;
		vec2 min_dir;
		if (right && !is_water(cur+vec2(1,0))  && right->height() < min_height) {
			min_height = right->height();  min_dir = vec2(1,0);
		}
		if (left  && !is_water(cur+vec2(-1,0)) && left->height()  < min_height) {
			min_height = left->height();   min_dir = vec2(-1,0);
		}
		if (up    && !is_water(cur+vec2(0,1))  && up->height()    < min_height) {
			min_height = up->height();     min_dir = vec2(0,1);
		}
		if (down  && !is_water(cur+vec2(0,-1)) && down->height()  < min_height) {
			min_height = down->height();   min_dir = vec2(0,-1);
		}

		if (min_dir.x == 0 && min_dir.y == 0) {
			break;
		}
		else {
			cur += min_dir;
		}
	}
}

void TerrainGenerator::generate_deep_water() {
	for (int x = 1; x < board_->width()-1; x++) {
		for (int y = 1; y < board_->height()-1; y++) {
			if (is_water(vec2(x,y)) 
					&& is_water(vec2(x-1,y))
					&& is_water(vec2(x+1,y))
					&& is_water(vec2(x,y-1))
					&& is_water(vec2(x,y+1))) {
				place_cell(vec2(x,y), new DeepWaterCell(height(vec2(x,y))));
			}
		}
	}
}

bool TerrainGenerator::is_water(vec2 pos) {
	Cell* cell = board_->get_cell(pos);
	return cell && (dynamic_cast<ShallowWaterCell*>(cell) 
	             || dynamic_cast<DeepWaterCell*>(cell));
}

void TerrainGenerator::generate_road(bool horiz, int width, int pos) {
	if (horiz) {
		for (int x = 0; x < board_->width(); x++) {
			for (int y = pos; y < pos + width; y++) {
				place_cell(vec2(x, y % board_->height()), new RoadCell(height(vec2(x, y % board_->height()))));
			}
		}
	}
	else {
		for (int y = 0; y < board_->height(); y++) {
			for (int x = pos; x < pos + width; x++) {
				place_cell(vec2(x % board_->width(), y), new RoadCell(height(vec2(x % board_->width(), y))));
			}
		}
	}
}
