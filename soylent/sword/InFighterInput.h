#ifndef InFighterInput_h
#define InFighterInput_h

#include "Fighter.h"

#include "Controllable.h"

class InFighterInput : public Controllable
{
public:
    InFighterInput(Fighter* fighter) 
        : fighter_(fighter), mSH(0), mSV(0)
    { }
    
	virtual void AddCommandSpecs(CommandSpecContainer *aCsc)
	{
		mUcHorizontalStance = aCsc->AddCommandSpec(new CommandSpec("horizontal-block"));
		mUcVerticalStance = aCsc->AddCommandSpec(new CommandSpec("vertical-block"));
		mUcJabStance = aCsc->AddCommandSpec(new CommandSpec("Jab Stance"));
		mUcBlockStance = aCsc->AddCommandSpec(new CommandSpec("Block Stance"));

		mUcLeft = aCsc->AddCommandSpec(new CommandSpec("Sword Left"));
		mUcRight = aCsc->AddCommandSpec(new CommandSpec("Sword Right"));
		mUcUp = aCsc->AddCommandSpec(new CommandSpec("Sword Up"));
		mUcDown = aCsc->AddCommandSpec(new CommandSpec("Sword Down"));

		mUcMsForward = aCsc->AddCommandSpec(new CommandSpec("MeatSack Forward"));
		mUcMsBack = aCsc->AddCommandSpec(new CommandSpec("MeatSack Back"));
		mUcMsLeft = aCsc->AddCommandSpec(new CommandSpec("MeatSack Left"));
		mUcMsRight = aCsc->AddCommandSpec(new CommandSpec("MeatSack Right"));

		mUcAuxUp = aCsc->AddCommandSpec(new CommandSpec("Aux Up"));
		mUcAuxDown = aCsc->AddCommandSpec(new CommandSpec("Aux Down"));
		mUcAuxLeft = aCsc->AddCommandSpec(new CommandSpec("Aux Left"));
		mUcAuxRight = aCsc->AddCommandSpec(new CommandSpec("Aux Right"));

		mUcRecording = aCsc->AddCommandSpec(new CommandSpec("Start/Stop Recording", 1000));
		mUcPlayback = aCsc->AddCommandSpec(new CommandSpec("Start/Stop Playback", 1000));
	}

	virtual void ExecuteCommand (Command *aC)
	{
		if (aC->mUcId == mUcJabStance)
		{
			if (aC->mFAmt == .0 && mSHcs)
			{
				remove_stance(mSHcs);
				mSHcs = 0;
			}
			else if (!mSHcs)
			{
				mSHcs = new HemiJabStance;
	            enter_temp_stance(mSHcs);
			}
		}
		else if (aC->mUcId == mUcBlockStance)
		{
			if (aC->mFAmt == .0 && mSGbs)
			{
				remove_stance(mSGbs);
				mSGbs = 0;
			}
			else if (!mSGbs)
			{
				mSGbs = new GeneralBlockStance;
	            enter_temp_stance(mSGbs);
			}
		}
		else if (aC->mUcId == mUcHorizontalStance)
		{
			if (aC->mFAmt == .0 && mSH)
			{
				remove_stance(mSH);
				mSH = 0;
			}
			else if (!mSH)
			{
				mSH = new HorizontalBlockStance;
	            enter_temp_stance(mSH);
			}
		}
		else if (aC->mUcId == mUcVerticalStance)
		{
			if (aC->mFAmt == .0 && mSV)
			{
				remove_stance(mSV);
				mSV = 0;
			}
			else if (!mSV)
			{
				mSV = new VerticalBlockStance;
	            enter_temp_stance(mSV);
			}
		}
		else if (aC->mUcId == mUcLeft)
		{
			if (aC->mBRelative)
		        fighter_->move_sword(vec2(aC->mFAmt * -.5, .0));
			else
		        fighter_->set_sword_x(-aC->mFAmt);
		}
		else if (aC->mUcId == mUcRight)
		{
			if (aC->mBRelative)
		        fighter_->move_sword(vec2(aC->mFAmt * .5, .0));
			else
		        fighter_->set_sword_x(aC->mFAmt);
		}
		else if (aC->mUcId == mUcUp)
		{
			if (aC->mBRelative)
		        fighter_->move_sword(vec2(.0, aC->mFAmt * .5));
			else
		        fighter_->set_sword_y(aC->mFAmt);
		}
		else if (aC->mUcId == mUcDown)
		{
			if (aC->mBRelative)
		        fighter_->move_sword(vec2(.0, aC->mFAmt * -.5));
			else
		        fighter_->set_sword_y(-aC->mFAmt);
		}
		else if (aC->mUcId == mUcMsForward)
		{
			if (aC->mBRelative)
		        fighter_->move_meatsack(vec3(.0, .0, aC->mFAmt * -.05));
			else
		        fighter_->move_meatsack(vec3(.0, .0, aC->mFAmt * -.05));
		}
		else if (aC->mUcId == mUcMsBack)
		{
			if (aC->mBRelative)
		        fighter_->move_meatsack(vec3(.0, .0, aC->mFAmt * .05));
			else
		        fighter_->move_meatsack(vec3(.0, .0, aC->mFAmt * .05));
		}
		else if (aC->mUcId == mUcMsLeft)
		{
			if (aC->mBRelative)
		        fighter_->move_meatsack(vec3(aC->mFAmt * -.05, .0, .0));
			else
		        fighter_->move_meatsack(vec3(aC->mFAmt * -.05, .0, .0));
		}
		else if (aC->mUcId == mUcMsRight)
		{
			if (aC->mBRelative)
		        fighter_->move_meatsack(vec3(aC->mFAmt * .05, .0, .0));
			else
		        fighter_->move_meatsack(vec3(aC->mFAmt * .05, .0, .0));
		}
		else if (aC->mUcId == mUcAuxLeft)
		{
			if (aC->mBRelative)
		        fighter_->move_aux(vec2(aC->mFAmt * -.5, .0));
			else
		        fighter_->set_aux_x(-aC->mFAmt);
		}
		else if (aC->mUcId == mUcAuxRight)
		{
			if (aC->mBRelative)
		        fighter_->move_aux(vec2(aC->mFAmt * .5, .0));
			else
		        fighter_->set_aux_x(aC->mFAmt);
		}
		else if (aC->mUcId == mUcAuxUp)
		{
			if (aC->mBRelative)
		        fighter_->move_aux(vec2(.0, aC->mFAmt * .5));
			else
		        fighter_->set_aux_y(aC->mFAmt);
		}
		else if (aC->mUcId == mUcAuxDown)
		{
			if (aC->mBRelative)
		        fighter_->move_aux(vec2(.0, aC->mFAmt * -.5));
			else
		        fighter_->set_aux_y(-aC->mFAmt);
		}
		else if (aC->mUcId == mUcRecording)
		{
			if (fighter_->IsRecording())
				fighter_->StopRecording();
			else
				fighter_->StartRecording();

		}
		else if (aC->mUcId == mUcPlayback)
		{
			if (fighter_->IsReplaying())
				fighter_->StopPlayback();
			else
				fighter_->LoopPlayback();
		}
	}

    void move(int x, int y) {
        fighter_->move_sword(vec2(0.01 * x, -0.01 * y));
    }

    void enter_temp_stance(ptr<Stance> stance) {
        restore_stance_.push_front(fighter_->change_stance(stance));
    }

	void remove_stance(ptr<Stance> stance) 
	{
        restore_stance_.remove(stance);
        if (!restore_stance_.empty()) 
		{
            fighter_->change_stance(restore_stance_.front());
        }
    }

    // subclasses override the event handlers, calling move() from within.
    // however, they should chain call their parents.


protected:

    Fighter* fighter_;
	std::list< ptr<Stance> > restore_stance_;
	ptr<Stance> mSH;
	ptr<Stance> mSV;
	ptr<Stance> mSHcs;
	ptr<Stance> mSGbs;
private:
	unsigned char mUcHorizontalStance;
	unsigned char mUcVerticalStance;
	unsigned char mUcJabStance;
	unsigned char mUcBlockStance;
	// Move sword up, down, left, right.
	unsigned char mUcLeft;
	unsigned char mUcRight;
	unsigned char mUcUp;
	unsigned char mUcDown;
	// Move meatsack (and therefore sword) forward, back, left, right.
	unsigned char mUcMsForward;
	unsigned char mUcMsBack;
	unsigned char mUcMsLeft;
	unsigned char mUcMsRight;
	// Aux axes.  Used for whatever extra stuff we might want in a stance.
	unsigned char mUcAuxUp;
	unsigned char mUcAuxDown;
	unsigned char mUcAuxLeft;
	unsigned char mUcAuxRight;
	// Start/Stop movement recording.  Start/Stop movement playback.
	unsigned char mUcRecording;
	unsigned char mUcPlayback;
};

#endif
