#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <soy/vec2.h>
#include <vector>
#include <list>
#include <set>
#include "UnitSpeedAptitude.h"

class PathPos {
public:
	PathPos(vec2 p, num weight) : x_(int(p.x)), y_(int(p.y)), weight_(weight) { }
	
	vec2 get_pos() const { return vec2(x_,y_); }
	num get_weight() const { return weight_; }
	void set_weight(num w) const { weight_ = w; }
	
	bool operator < (const PathPos& other) const {
		return x_ <  other.x_ 
			|| x_ == other.x_ && y_ < other.y_;
		// not comparing weight; it is added as annotation
	}
	
private:
	int x_, y_;
	mutable num weight_;  // this is OK because it is annotation, and doesn't
	                      // participate in the compare
};

class PathMeasurer;

class PathFind {
public:
	PathFind() { }
	
	void set_dest(vec2 dest);
	void set_speed_apt(const UnitSpeedAptitude& speed) { speed_ = speed; }

	num get_weight(vec2 pos);
	
private:
	// Returns false if the node has already been seen with a smaller weight
	bool visit(const PathPos& pos, const PathMeasurer& measurer);
	
	vec2 dest_;
	UnitSpeedAptitude speed_;

	typedef std::set<PathPos> closed_t;
	closed_t closed_;
	typedef std::vector<PathPos> queue_t;
	queue_t queue_;
};



#endif
