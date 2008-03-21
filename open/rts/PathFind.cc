#include "PathFind.h"
#include "config.h"
#include "Board.h"
#include <soy/Timer.h>
#include <set>
#include <algorithm>

struct PathMeasurer {
public:
	PathMeasurer(vec2 src, num max_speed) : src_(src), max_speed_(max_speed) { }
	
	bool operator () (const PathPos& l, const PathPos& r) {
		return l.get_weight() + (l.get_pos() - src_).norm() / max_speed_
			 > r.get_weight() + (r.get_pos() - src_).norm() / max_speed_;
	}
private:
	vec2 src_;
	num max_speed_;
};

void PathFind::set_dest(vec2 dest) {
	closed_.clear();
	queue_.clear();
	
	dest_ = dest;
	PathPos init(dest_, 0);
	queue_.push_back(init);
}

num PathFind::get_weight(vec2 src) {
	PathMeasurer measurer(src, speed_.get_max_speed());

	// Check to see if we already have the answer, then we don't
	// have to sort.
	{
		closed_t::const_iterator i = closed_.find(PathPos(src, 0));
		if (i != closed_.end()) {
			return i->get_weight();
		}
	}

	// Don't even attempt to calculate the weight for impassible squares...
	// you know what the answer will be
	if (BOARD.calculate_speed(src, speed_) <= 0) {
		return HUGE_VAL;
	}

	// Sort the existing queue based on the new measurer
	std::make_heap(queue_.begin(), queue_.end(), measurer);
	
	int iters = 0;
	while (!queue_.empty()) {
		// XXX I cannot figure out how to limit something in step()
		// based on time, so I will stupidly constrain the algorithm to 
		// 1,000 iterations (it is iterative, so progress will be made
		// each frame).
		if (++iters > 1000) {
			break;
		}
				
		// Get the least-weighted node from the heap
		PathPos top = queue_.front();
		std::pop_heap(queue_.begin(), queue_.end(), measurer);
		queue_.pop_back();
		
		// Check to see if the node has been visisted
		if (!visit(top, measurer)) continue;

		// If we're looking at the destination position, then we're done.
		if (int(top.get_pos().x) == int(src.x) && int(top.get_pos().y) == int(src.y)) {
			break;
		}

		// Look at all neighboring cells
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				vec2 dp = top.get_pos() + vec2(dx,dy);
				// Check bounds
				if (dp.x < 0 || dp.x >= BOARD.width() || dp.y <= 0 || dp.y >= BOARD.width() 
						|| dx == 0 && dy == 0) continue;
				
				num speed = BOARD.calculate_speed(dp, speed_);
				if (abs(dx) + abs(dy) == 2) {
					// We must not traverse diagonals with impassible sides.
					// If they have low speeds, they will be neglegable, but
					// when they are zero they are impassible.
					num speedx = BOARD.calculate_speed(vec2(dp.x, top.get_pos().y), speed_);
					num speedy = BOARD.calculate_speed(vec2(top.get_pos().x, dp.y), speed_);
					if (speedx <= 0 || speedy <= 0) {
						speed = 0;
					}
				}

				if (speed > 0) {
					// Create a new node and append it to the current path
					PathPos nextpos(dp, top.get_weight() + (top.get_pos() - dp).norm() / speed);

					queue_.push_back(nextpos);
					std::push_heap(queue_.begin(), queue_.end(), measurer);
				}
			}
		}
	}

	{
		closed_t::const_iterator i = closed_.find(PathPos(src, 0));
		if (i != closed_.end()) {
			return i->get_weight();
		}
		else {
			if (iters > 1000) { // we bailed due to the iteration constraint
				return (src - dest_).norm() / speed_.get_max_speed();
			}
			else {              // couldn't find a path here
				return HUGE_VAL;
			}
		}
	}
}

bool PathFind::visit(const PathPos& pos, const PathMeasurer& measurer)
{
	closed_t::iterator i = closed_.find(pos);
	if (i == closed_.end()) {
		closed_.insert(pos);
		return true;
	}
	else if (pos.get_weight() < i->get_weight()) {
		i->set_weight(pos.get_weight());
		return true;
	}

	return false;
}
