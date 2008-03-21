#ifndef __UNITSPEEDAPTITUDE_H__
#define __UNITSPEEDAPTITUDE_H__

#include <map>
#include "Cell.h"

class UnitSpeedAptitude {
public:
	UnitSpeedAptitude() 
		: turn_(HUGE_VAL), max_speed_(0)
		, accel_(HUGE_VAL), decel_(HUGE_VAL) 
	{ }
	
	num apt(TerrainType type) const {
		apt_t::const_iterator i = apt_.find(type);
		if (i != apt_.end()) {
			return i->second;
		}
		else {
			return 0;
		}
	}

	void set_apt(TerrainType type, num in) {
		if (in > max_speed_) max_speed_ = in;
		apt_[type] = in;
	}

	num get_turn_rate() const { return turn_; }      // rad/s
	void set_turn_rate(num in) { turn_ = in; }

	num get_acceleration() const { return accel_; }  // unit/s^2
	void set_acceleration(num accel) { accel_ = accel; }

	num get_deceleration() const { return decel_; }  // unit/s^2
	void set_deceleration(num decel) { decel_ = decel; }

	num get_max_speed() const { return max_speed_; } // unit/s

private:
	typedef std::map< TerrainType, num > apt_t;
	apt_t apt_;
	num turn_;
	num max_speed_;
	num accel_;
	num decel_;
};

#endif
