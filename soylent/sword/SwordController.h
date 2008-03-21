#ifndef __SWORDCONTROLLER_H__
#define __SWORDCONTROLLER_H__

#include "Sword.h"

extern SoyInit INIT;

class SwordController
{
public:
    SwordController(Sword* sword) : sword_(sword), 
		tip_target_(0, 0, SWORD_LENGTH),
		handle_target_(0, 0, 0),
		time_(0)
	{ }

    // the time parameter says that you would like the tip
    // to be at pos "time" seconds from now
    void move(vec3 handle, vec3 tip, double time) {
        handle_target_ = handle;
        tip_target_ = tip;
        time_ = time;
    }

    void step(const double &max_force) 
	{
        vec3 handle_vel;

		//time_ = 0.1;
		const double min_time(.05);
        if (time_ < min_time) time_ = min_time;

        if (time_ > 0) 
		{
            // how fast the handle needs to be going to get there in time
            handle_vel = (handle_target_ - sword_->get_handle_position()) / time_;
        }
        // how much force we need to apply to get the handle going that speed
        vec3 handle_force = 0.5 * (handle_vel - sword_->get_handle_velocity()) * sword_->get_mass() / DT;

        vec3 tip_vel;
        if (time_ > 0) {
            // how fast the tip needs to be going to get there in time
            tip_vel = (tip_target_ - sword_->get_tip_position()) / time_;
        }
        // okay, to do this precisely is some hard math, so let's just guess...
        vec3 tip_force = 0.5 * (tip_vel - sword_->get_tip_velocity()) * sword_->get_mass() / DT;

        vec3 backforce = handle_force * 0.5 - tip_force * 4; // these coefficients (4 and 5) are
        vec3 foreforce = handle_force * 0.5 + tip_force * 5; // somehow dependent on the sword balancing
                                                             // the latter minus the former *must*
                                                             // be equal to one

        double scale = 1;
        if (backforce.norm() > max_force) {
            scale = max_force / backforce.norm();
        }
        if (foreforce.norm() > max_force) {
            if (max_force / foreforce.norm() > scale) {
                foreforce = max_force / foreforce.norm();
            }
        }
        
        sword_->apply_force_backhand(scale*backforce);
        sword_->apply_force_forehand(scale*foreforce);

        time_ -= DT;

        sword_->step();
    }

private:
    Sword* sword_;
    vec3 tip_target_;
    vec3 handle_target_;
    double time_;
};

#endif
