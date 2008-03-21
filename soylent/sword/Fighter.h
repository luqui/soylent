#ifndef __FIGHTER_H__
#define __FIGHTER_H__

#include <soy/init.h>
#include <soy/util.h>
#include <soy/ptr.h>
#include "SwordController.h"
#include "Stance.h"
#include "MeatSack.h"
#include "MeatSackController.h"

#include <fstream>

class Fighter
{
public:
    Fighter(Stance *aS, vec3 pos, vec3 facing) 
        : sword_controller_(&sword_)
        , meatsack_controller_(&meatsack_)
		, stance_(aS)
		, loop_playback_(false)
    {
		set_facing_direction(facing);

        meatsack_.set_position(pos);
		meatsack_move_ = vec3(.0, .0, .0);

		sword_pos_ = stance_->natural_position();
		aux_pos_ = vec2(.0, .0);

		Stance::Positions ps = stance_->get_positions(sword_pos_, aux_pos_);
		vec3 handle, tip;
		handle = rot_.rotate(ps.handle) + meatsack_.get_position();
		tip = rot_.rotate(ps.tip) + meatsack_.get_position();
        sword_.set_position((handle + tip) / 2.0);
		sword_.set_orientation(rot_);
    }

    void draw() const {
        sword_.draw();
        meatsack_.draw();
    }

    void step() 
	{
		// if we're recording, record the pos vectors.
		if (IsRecording())
		{
			recording_ << meatsack_move_.x << " " << meatsack_move_.y << " " << meatsack_move_.z << " " <<
						sword_pos_.x << " " << sword_pos_.y << " " << 
						aux_pos_.x << " " << aux_pos_.y << std::endl;
		}

		// If replaying, replace current vectors with the recorded ones.
		if (IsReplaying())
		{
			if (playback_.eof())
			{
				playback_.close();
				// clear status flags, so we can reopen.
				playback_.clear();		
				if (loop_playback_)
					playback_.open(playbackfile_.c_str());
			}
			else
				playback_ >> meatsack_move_.x >> meatsack_move_.y >> meatsack_move_.z >> 
						sword_pos_.x >> sword_pos_.y >> 
						aux_pos_.x >> aux_pos_.y;
		}

		// if the meatsack is dead, go with the dead stance.
		// Not particularly compatable with the stances in InFighterInput, etc.
		if (meatsack_.get_hit_countdown() > 0 && meatsack_.get_fatal())
		{
			if (!mSDead)
			{
				mSDefault = stance_;
				mSDead = new DeadStance;
				stance_ = mSDead;
			}
		}
		else if (!mSDead.null())
		{
			// When the countdown is up, revert.
			mSDead = 0;
			stance_ = mSDefault;
		}

		// update the fighter pos and dir from the meatsack positions.
		meatsack_move_ = rot_.rotate(meatsack_move_) + meatsack_.get_position();
		meatsack_controller_.move(meatsack_move_);
		meatsack_move_ = vec3(.0, .0, .0);
		meatsack_controller_.step(500);

        Stance::Positions ps = stance_->get_positions(sword_pos_, aux_pos_);
        sword_controller_.move(rot_.rotate(ps.handle) + meatsack_.get_position(), rot_.rotate(ps.tip) + meatsack_.get_position(), .25);
        sword_controller_.step(stance_->get_max_force());
    }

    ptr<Stance> change_stance(ptr<Stance> stance) {
        ptr<Stance> ret = stance_;
        stance_ = stance;
        stance_->enter(meatsack_.get_position(), rot_, &sword_);
        return ret;
    }


    // x,y in [-1,1]
    void move_sword(vec2 amt) {
        sword_pos_ += amt;
        if (sword_pos_.norm2() > 1) {
            sword_pos_ = ~sword_pos_;
        }
    }
    void set_sword_pos(vec2 pos) {
        sword_pos_ = pos;
    }
    void set_sword_x(const double &x) {
        sword_pos_.x = x;
    }
    void set_sword_y(const double &y) {
        sword_pos_.y = y;
    }

    void move_aux(vec2 amt) {
        aux_pos_ += amt;
        if (aux_pos_.norm2() > 1) {
            aux_pos_ = ~aux_pos_;
        }
    }
    void set_aux_pos(vec2 pos) {
        aux_pos_ = pos;
    }
    void set_aux_x(const double &x) {
        aux_pos_.x = x;
    }
    void set_aux_y(const double &y) {
        aux_pos_.y = y;
    }

	void move_meatsack(vec3 amt)
	{
		meatsack_move_ += amt;
	}
	vec3 get_meatsack_pos() const { return meatsack_.get_position(); }
	const Quaternion& get_meatsack_rot() const { return rot_; }		// ms rot == player rot.

	void set_facing_direction(vec3 facing)
	{
        dQuaternion q;
        vec3 axis = vec3(0, 0, -1) % facing;
        double angle = acos(vec3(0, 0, -1) * facing / facing.norm());
        dQFromAxisAndAngle(q, axis.x, axis.y, axis.z, angle);
        rot_ = q;
	}

	// inline const vec3& get_pos() const { return pos_; }

	// Movement recording/playback functions.
	void StartRecording();
	void StopRecording();
	bool IsRecording();
	void Replay();
	void LoopPlayback(const char *file = "recording.txt");
	void StopPlayback();
	bool IsReplaying();

	inline const MeatSack* get_meatsack() const { return &meatsack_; }
	inline MeatSack* get_meatsack() { return &meatsack_; }
	inline const Sword* get_sword() const { return &sword_; }
private:
    //vec3 pos_;
    Quaternion rot_;

	ptr<Stance> mSDead;
	ptr<Stance> mSDefault;

	vec3 meatsack_move_;

	vec2 sword_pos_;
	vec2 aux_pos_;		// another 'pos' for some experimental stances.
    Sword sword_;
    SwordController sword_controller_;
    MeatSack meatsack_;
	MeatSackController meatsack_controller_;
    ptr<Stance> stance_;

	// member vars for movement recording/playback.
	std::ofstream recording_;
	std::ifstream playback_;
	std::string playbackfile_;
	bool loop_playback_;
};

#endif
