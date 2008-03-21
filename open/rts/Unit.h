#ifndef __UNIT_H__
#define __UNIT_H__

#include <soy/vec2.h>
#include <soy/Texture.h>
#include <cmath>
#include "UnitSpeedAptitude.h"
#include "util.h"
#include "config.h"
#include "Board.h"
#include "Dust.h"
#include "AI.h"
#include "Cell.h"

class Unit
{
public:
	Unit() : cell_(NULL), cell_prev_(NULL), cell_next_(NULL) { }
	virtual ~Unit() {
		exit_cell();
	}

	virtual void draw() const = 0;
	virtual vec2 get_position() const = 0;
	virtual BoundingBox get_bounding_box() const = 0;
	virtual void step()           { ai_.step(); }
	virtual void set_ai(VALUE ai) { ai_.set(ai); }
	virtual VALUE get_ai() const  { return ai_.get(); }


	Unit* next_unit() const { return cell_next_; }
	Cell* current_cell() const { return cell_; }
protected:
	void exit_cell() {
		if (cell_) {
			if (cell_->unit_head_ == this) cell_->unit_head_ = cell_next_;
			if (cell_prev_) cell_prev_->cell_next_ = cell_next_;
			if (cell_next_) cell_next_->cell_prev_ = cell_prev_;
			cell_ = NULL;
			cell_prev_ = cell_next_ = NULL;
		}
	}

	void enter_cell(Cell* c) {
		if (!c) {
			exit_cell();
		}
		else if (c != cell_) {
			exit_cell();
			cell_ = c;
			cell_prev_ = NULL;
			cell_next_ = cell_->unit_head_;
			cell_->unit_head_ = this;
		}
	}
private:
	AI ai_;
	Cell* cell_;
	Unit* cell_prev_;
	Unit* cell_next_;
};


inline Unit* unit_at(BoundingBox pos, Unit* except) {
	for (std::list<Unit*>::const_iterator i = UNITS.begin(); i != UNITS.end(); ++i) {
		if (*i != except && intersects((*i)->get_position(), pos)) {
			return *i;
		}
	}
	return NULL;
}


class MobileUnit : virtual public Unit
{
public:
	virtual ~MobileUnit() { }

	virtual void set_velocity(vec2 in) = 0;
	virtual vec2 get_velocity() const = 0;
	virtual UnitSpeedAptitude get_speed_aptitude() const = 0;
	virtual bool obstructed() const = 0;
	virtual vec2 nudge(vec2 dp) = 0;
	virtual num get_angle() const = 0;
};


class VehicleUnit : virtual public MobileUnit
{
public:
	VehicleUnit(vec2 pos) : obstructed_(false), turning_(false), pos_(pos), speed_(0), 
	                        target_speed_(0), theta_(0), target_theta_(0)
	{
		enter_cell(BOARD.get_cell(pos_));
	}

	~VehicleUnit() { }

	vec2 get_position() const {
		return pos_;
	}

	void step() {
		MobileUnit::step();
		turning_ = false;
		
		obstructed_ = false;

		Cell* cell = BOARD.get_cell(pos_);
		enter_cell(cell);

		if (cell && dynamic_cast<DirtCell*>(cell) && speed_ > 0.2) {
			DUST.add_dust(pos_);
		}

		if (fabs(theta_ + 2*M_PI - target_theta_) < fabs(theta_ - target_theta_)) {
			theta_ += 2*M_PI;
		}
		else if (fabs(theta_ - 2*M_PI - target_theta_) < fabs(theta_ - target_theta_)) {
			theta_ -= 2*M_PI;
		}
		
		if (fabs(theta_ - target_theta_) > DT * get_speed_aptitude().get_turn_rate()) {
			theta_ += DT * get_speed_aptitude().get_turn_rate() * (target_theta_ - theta_ < 0 ? -1 : 1);
			turning_ = true;
		}
		else {
			theta_ = target_theta_;
		}

		num max_speed = BOARD.calculate_speed(pos_, apt_);
		num tspeed = target_speed_ > max_speed ? max_speed : target_speed_;

		if (tspeed - speed_ > DT * get_speed_aptitude().get_acceleration()) {
			speed_ += DT * get_speed_aptitude().get_acceleration();
		}
		else if (tspeed - speed_ < -DT * get_speed_aptitude().get_deceleration()) {
			speed_ -= DT * get_speed_aptitude().get_deceleration();
		}
		else {
			speed_ = tspeed;
		}
		
		num offness = acos(cos(theta_ - target_theta_));
		num scale = (M_PI - offness) / M_PI;

		if (max_speed <= 0) {
			// get him out of there! (XXX does this work?)
			pos_ = vec2(int(pos_.x), int(pos_.y));
		}
		
		vec2 vel = speed_ * scale * vec2(cos(theta_), sin(theta_));
		vec2 dp = DT * vel;
		
		nudge(DT * vel);
	}

	vec2 nudge(vec2 dp) {
		if (dp.x == 0 && dp.y == 0) return vec2();
		vec2 hypx = pos_ + vec2(dp.x, 0);
		vec2 hypy = pos_ + vec2(0, dp.y);

		if (Unit* hypxu = unit_at(BoundingBox(hypx - vec2(0.5,0.5), hypx + vec2(0.5,0.5)), this)) {
			if ((hypxu->get_position().x - pos_.x) * dp.x > 0) {
				if (MobileUnit* hypxum = dynamic_cast<MobileUnit*>(hypxu)) {
					dp.x = hypxum->nudge(vec2(dp.x,0)).x;
				}
				else {
					dp.x = 0;
				}
			}
			else {
				dp.x = 0;
			}
			obstructed_ = !turning_;
		}
		if (BOARD.calculate_speed(hypx, apt_) <= 0) {
			dp.x = 0;
			obstructed_ = !turning_;
		}
		
		if (Unit* hypyu = unit_at(BoundingBox(hypy - vec2(0.5,0.5), hypy + vec2(0.5,0.5)), this)) {
			if ((hypyu->get_position().y - pos_.y) * dp.y > 0) {
				if (MobileUnit* hypyum = dynamic_cast<MobileUnit*>(hypyu)) {
					dp.y = hypyum->nudge(vec2(0,dp.y)).y;
				}
				else {
					dp.y = 0;
				}
			}
			else {
				dp.y = 0;
			}
			obstructed_ = !turning_;
		}
		if (BOARD.calculate_speed(hypy, apt_) <= 0 ) {
			dp.y = 0;
			obstructed_ = !turning_;
		}

		pos_ += dp;
		return dp;
	}

	void set_velocity(vec2 in) {
		if (in.norm2() > 0) {
			target_theta_ = atan2(in.y, in.x);
		}
		target_speed_ = in.norm();
	}

	vec2 get_velocity() const {
		return speed_ * vec2(cos(theta_), sin(theta_));
	}

	num get_angle() const {
		return theta_;
	}

	UnitSpeedAptitude get_speed_aptitude() const { 
		return apt_; 
	}

	bool obstructed() const { return obstructed_; }
	
protected:
	bool obstructed_;
	bool turning_;
	UnitSpeedAptitude apt_;
	vec2 pos_;
	num speed_;
	num target_speed_;
	num theta_;
	num target_theta_;
};

#endif
