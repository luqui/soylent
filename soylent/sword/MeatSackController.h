#ifndef __MEATSACKCONTROLLER_H__
#define __MEATSACKCONTROLLER_H__

#include "MeatSack.h"

extern SoyInit INIT;

class MeatSackController
{
public:
    MeatSackController(MeatSack* meatsack) : meatsack_(meatsack) { }

    // the time parameter says that you would like the meatsack position
    // to be at pos "time" seconds from now
    void move(vec3 pos, double time = .0) 
	{
        pos_target_ = pos;
        time_ = time;
    }

    void step(const double &max_force) 
	{
        vec3 pos_vel;

        if (time_ < 0.05) time_ = 0.05;

        if (time_ > 0) {
            // how fast the pos needs to be going to get there in time
            pos_vel = (pos_target_ - meatsack_->get_position()) / time_;
        }
        // how much force we need to apply to get the pos going that speed
        vec3 pos_force = 0.5 * (pos_vel - meatsack_->get_velocity()) * meatsack_->get_mass() / DT;

        double scale = 1;
        if (pos_force.norm() > max_force) 
		{
            scale = max_force / pos_force.norm();
        }
        //if (foreforce.norm() > max_force) {
        //    if (max_force / foreforce.norm() > scale) {
        //        foreforce = max_force / foreforce.norm();
        //    }
        //}

		// A little vertical hop to help move along.
		pos_force.y = .01;
        
        meatsack_->apply_force(pos_force * scale);

        time_ -= DT;

        meatsack_->step();
    }

private:
    MeatSack* meatsack_;
    vec3 pos_target_;
    double time_;
};

#endif
