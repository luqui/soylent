#ifndef __STANCE_H__
#define __STANCE_H__

#include "physics.h"
#include "Sword.h"
#include "MeatSack.h"
#include <soy/vec3.h>
#include <soy/vec2.h>
#include <string>
#include <NVThing.h>
#include <list>
#include <Exception.h>

using namespace bburdette;

inline double interpolate(const double &from, const double &afrom, const double &bfrom, const double &ato, const double &bto)
{
	return ((from - afrom) * (bto - ato) )/ (bfrom - afrom) + ato;
}

class Stance
{
public:
    struct Positions {
        // x = left-right
        // y = down-up
        // z = forward-back  (i.e. forward is NEGATIVE)
        //     (this is to keep it a right-handed coordinate system)
        vec3 handle;
        vec3 tip;
    };

	Stance(const char* aName) { max_force = 500; name = aName; }
    
    virtual ~Stance() { }

	const char* GetName() const { return name.c_str(); }

	virtual Stance* Duplicate() const = 0;

	inline const double& get_max_force() const { return max_force; }

    // x,y in [-1,1]
    virtual void enter(vec3 pos, Quaternion rot, Sword* sword) { }
    virtual Positions get_positions(vec2 pos) const = 0;
    virtual Positions get_positions(const vec2 &pos, const vec2 &auxpos) const
	{
		return get_positions(pos);		// by default use the single arg version.
	}

    virtual vec2 natural_position() const 
	{
        return vec2(0,0);
    }
protected:
	double max_force;
	mutable Positions old_pos;
	std::string name;
};

class StanceBank
{
public:
	void AddStance(Stance *aS)
	{
		mLStances.push_back(aS);
	}

	// Static instance of the stancebank.
	static StanceBank sSbTheStanceBank;

	static void SAddStance(Stance *aS)
	{
		sSbTheStanceBank.AddStance(aS);
	}

	static Stance* SMakeStance(const NVThing &aNvtPrefs, const char *aCStanceField)
	{
		return sSbTheStanceBank.MakeStance(aNvtPrefs, aCStanceField);
	}

	Stance* MakeStance(const NVThing &aNvtPrefs, const char *aCStanceField)
	{
		const char *lC;
		if (!aNvtPrefs.Find(aCStanceField, lC))
		{
			throw MessageException("Stance Field not found in prefs");
		}

		return MakeStance(lC);
	}

	Stance* MakeStance(const char *aCStanceName)
	{
		list<Stance*>::const_iterator lIter = mLStances.begin();
		while (lIter != mLStances.end())
		{
			if (strcmpi(aCStanceName, (*lIter)->GetName()) == 0)
				return (*lIter)->Duplicate();
			++lIter;
		}
		throw MessageException("Stance not found");
	}
private:
	std::list<Stance*> mLStances;
};

class PlaneChudanStance : public Stance
{
public:
	PlaneChudanStance() 
		:Stance("PlaneChudanStance")
	{ 
	}

	virtual Stance* Duplicate() const { return new PlaneChudanStance; }

    Positions get_positions(vec2 pos) const {
        Positions ret;
		pos *= 2.0;
        ret.handle = vec3(pos.x/4, 1, -0.35);
        ret.tip = vec3(pos.x, 1+pos.y, -1);
        return ret;
    }
};

class HemiJabStance : public Stance
{
public:
	HemiJabStance() 
		:Stance("HemiJabStance")
	{ 
		max_force = 500; 
	}
	virtual Stance* Duplicate() const { return new HemiJabStance; }

	Positions get_positions(vec2 pos) const {
        Positions ret;

		// have y values range from -.4 to 1.0, not -1.0 to 1.0.
		pos.y = ((pos.y + 1.0) / 2.0) * 1.4 - .4;

		vec3 hposnorm;
		hposnorm.x = pos.x;
		hposnorm.y = pos.y;
		double lD = pos.norm2();
		if (lD < 1.0)
			hposnorm.z = -sqrt(1.0 - pos.norm2());
		else
			hposnorm.z = .0;

		vec3 hpos(hposnorm);

		hpos *= SWORD_LENGTH * .85;

		ret.handle = hpos;
		ret.tip = hpos + hposnorm * SWORD_LENGTH;

		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += MEATSACK_HEIGHT * .25;
        ret.handle.z += MEATSACK_FATNESS * -1.0;
		ret.tip.z += MEATSACK_FATNESS * -1.0;

        return ret;
    }
};

class HemiChudanStance : public Stance
{
public:
	HemiChudanStance() 
		:Stance("HemiChudanStance")
	{ max_force = 750; }

	virtual Stance* Duplicate() const { return new HemiChudanStance; }

    Positions get_positions(vec2 pos) const 
	{
		// Place the handle in a hemisphere.
		Positions ret;

		// have y values range from -.4 to 1.0, not -1.0 to 1.0.
		pos.y = ((pos.y + 1.0) / 2.0) * 1.4 - .4;

		vec3 hposnorm;
		hposnorm.x = pos.x;
		hposnorm.y = pos.y;
		double lD = pos.norm2();
		if (lD < 1.0)
			hposnorm.z = -sqrt(1.0 - pos.norm2());
		else
			hposnorm.z = .0;

		vec3 hpos(hposnorm);

		hpos *= SWORD_LENGTH * .4;

		ret.handle = hpos;

		// Place the tip along a line from vec3(0, 0, SWORD_LENGTH * -.3) through the handle point.
		//vec3 lV(hpos);
		//lV.z -= SWORD_LENGTH * -.3;
		//lV = ~lV;

		// Place the tip along a line from lV through the handle point.
		vec3 lV(hpos);
		lV -= vec3(hpos.x * .6, hpos.y * .6, SWORD_LENGTH * -.3);
		lV = ~lV;

		ret.tip = hpos + lV * SWORD_LENGTH;

		// Now move the whole mess up by 1.5 (hand height)
		// and backward by 1.
		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += MEATSACK_HEIGHT * .25;
        ret.handle.z += MEATSACK_FATNESS * -.8;
		ret.tip.z += MEATSACK_FATNESS * -.8;

        return ret;
    }
};

class GeneralBlockStance : public Stance
{
public:
	GeneralBlockStance() 
		:Stance("GeneralBlockStance")
	{ 
		max_force = 1000; 
		last_.x = 0;
		last_.y = 1.0;
		lastlen_ = 1.0;

		deadzone_ = .25;
	}

	virtual Stance* Duplicate() const { return new GeneralBlockStance; }

    Positions get_positions(vec2 pos) const 
	{
		Positions ret;

		double lD = pos.norm();

		vec2 orient;
		// If we're in the dead zone, use the last_ orientation.
		// otherwise, update it.
		if (lD > deadzone_)
		{
			// if a 180 flip is closer to last, use that instead of new orientation.
			if ((last_ - pos).norm2() < (last_ + pos).norm2())
				last_ = pos;
			else
				last_ = -pos;

			lastlen_ = lD;
		}

		// flip 90 degrees.
		orient.x = last_.y;
		orient.y = -last_.x;
		orient *= SWORD_LENGTH / lastlen_;

		ret.handle.x = pos.x - orient.x;
		ret.handle.y = pos.y - orient.y;
		ret.tip.x = pos.x + orient.x;
		ret.tip.y = pos.y + orient.y;

		// Now move the whole mess up by 1.5 (hand height)
		// and backward by .3.
		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += ret.handle.y;
        ret.handle.z += MEATSACK_FATNESS * -1. + SWORD_LENGTH * -.4;
		ret.tip.z += ret.handle.z;

        return ret;
    }
private:
	mutable vec2 last_;
	mutable double lastlen_;
	double deadzone_;
};

// This stance uses the 'aux' position for wrist orientation.
class WristStance : public Stance
{
public:
	WristStance(const char *aCStanceName = "WristStance") 
		:Stance(aCStanceName)
	{ max_force = 1100; }

	virtual Stance* Duplicate() const { return new WristStance; }

    Positions get_positions(vec2 pos) const
	{
		Positions ret;
		ret.handle = vec3(0, 0, 0);
		ret.tip = vec3(0, 0, 0);
		return ret;
	}

	Positions get_positions(const vec2 &pos, const vec2 &auxpos) const 
	{
		// Auxpos determines the wrist angle.  
		// Overall position comes from pos.

        Positions ret;
		// have y values range from -.4 to 1.0, not -1.0 to 1.0.
		//vec2 pos(apos.x, ((apos.y + 1.0) / 2.0) * 1.4 - .4);
		//vec2 auxpos(aauxpos.x, ((aauxpos.y + 1.0) / 2.0) * 1.4 - .4);

		// Map hilt position onto a hemisphere.  
		vec3 hposnorm;
		hposnorm.x = pos.x;
		hposnorm.y = pos.y;
		double lDHP = pos.norm2();
		if (lDHP < 1.0)
			hposnorm.z = -sqrt(1.0 - lDHP);
		else
			hposnorm.z = .0;

		// calc xform from straight forward to current hilt pos.
		Quaternion lQ;
		lQ.calcrotation(vec3(0, 0, -1), hposnorm);

		// Map wrist position onto a hemisphere.  
		vec3 wposnorm;
		wposnorm.x = auxpos.x;
		wposnorm.y = auxpos.y;
		double lD = auxpos.norm2();
		if (lD < 1.0)
			wposnorm.z = -sqrt(1.0 - lD);
		else
			wposnorm.z = .0;

		// Increase the angle of the wrist when you are close to the center of 
		// arm motion.  when you are at the edge of arm motion scale back to 
		// regular hemispherical wrist rotation.  This is to keep you from being able 
		// to hit yourself so easily, while making blocking work better in the center.
		vec3 lV(wposnorm);
		lV -= vec3(pos.x * .7, pos.y * .7, -.3) * ((lDHP < 1.0) ? 1.0 - lDHP : .0);
		wposnorm = ~lV;

		vec3 hpos(0, 0, -1);
		hpos *= SWORD_LENGTH * .75;	  

		ret.handle = vec3(0, 0, -1);
		ret.tip = ret.handle + wposnorm * SWORD_LENGTH;

		ret.handle = lQ.rotate(ret.handle);
		ret.tip = lQ.rotate(ret.tip);

		double lDZSub = ret.handle.z * .75;
		ret.handle.z -= lDZSub;
		ret.tip.z -= lDZSub;

		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += MEATSACK_HEIGHT * .25;
        ret.handle.z += MEATSACK_FATNESS * -1.0;
		ret.tip.z += MEATSACK_FATNESS * -1.0;

        return ret;
    }
};

// double hemispheres, one for tip pos and one for hilt pos.
// hilt is on hilt hemi, sword tip vec points to tip pos.
class HiltCentricHemis : public Stance
{
public:
	HiltCentricHemis() 
		:Stance("HiltCentricHemis")
	{ max_force = 1100; }

	virtual Stance* Duplicate() const { return new HiltCentricHemis; }

    Positions get_positions(vec2 pos) const
	{
		Positions ret;
		ret.handle = vec3(0, 0, 0);
		ret.tip = vec3(0, 0, 0);
		return ret;
	}

	Positions get_positions(const vec2 &pos, const vec2 &auxpos) const 
	{
		// Auxpos determines the wrist angle.  
		// Overall position comes from pos.

        Positions ret;

		// Map hilt position onto a hemisphere.  
		vec3 hposnorm;
		hposnorm.x = pos.x;
		hposnorm.y = pos.y;
		double lDHP = pos.norm2();
		if (lDHP < 1.0)
			hposnorm.z = -sqrt(1.0 - lDHP);
		else
			hposnorm.z = .0;

		// Map tip position onto a hemisphere.  
		vec3 tip;
		tip.x = auxpos.x;
		tip.y = auxpos.y;
		double lD = auxpos.norm2();
		if (lD < 1.0)
			tip.z = -sqrt(1.0 - lD);
		else
			tip.z = .0;

		tip *= SWORD_LENGTH + 1.0;

		vec3 swordvec = tip - hposnorm;
		swordvec = ~swordvec;

		swordvec *= SWORD_LENGTH;
		swordvec += hposnorm;

		ret.handle = hposnorm;
		ret.tip = swordvec;

		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += MEATSACK_HEIGHT * .25;
        ret.handle.z += MEATSACK_FATNESS * -1.0;
		ret.tip.z += MEATSACK_FATNESS * -1.0;

        return ret;

    }
};

// double hemispheres, one for tip pos and one for hilt pos.
// hilt is on hilt hemi, sword tip vec points to tip pos.
// this stance uses a relative mapping of the control axis
class HiltCentricRel : public Stance
{
public:
	HiltCentricRel()
		:Stance("HiltCentricRel")
	{ max_force = 1100; }

	virtual Stance* Duplicate() const { return new HiltCentricRel; }

    Positions get_positions(vec2 pos) const
	{
		Positions ret;
		ret.handle = vec3(0, 0, 0);
		ret.tip = vec3(0, 0, 0);
		return ret;
	}

	Positions get_positions(const vec2 &pos, const vec2 &auxpos) const 
	{
		// Auxpos determines the wrist angle.  
		// Overall position comes from pos.

		vec2 npos = pos;
		vec2 nauxpos = auxpos;

		const double DEADZONE = 0.15;
		const double SENSITIVITY = 0.005;

		if (npos.x < DEADZONE && npos.x > -DEADZONE) npos.x = 0;
		if (npos.y < DEADZONE && npos.y > -DEADZONE) npos.y = 0;
		if (nauxpos.x < DEADZONE && nauxpos.x > -DEADZONE) nauxpos.x = 0;
		if (nauxpos.y < DEADZONE && nauxpos.y > -DEADZONE) nauxpos.y = 0;

        Positions ret;

		// Map hilt position onto a hemisphere.  
		vec3 hposnorm;
		hposnorm.x = npos.x * SENSITIVITY + old_pos.handle.x;
		hposnorm.y = npos.y * SENSITIVITY + old_pos.handle.y;
		double lDHP = hposnorm.norm2();
		if (lDHP < 1.0)
			hposnorm.z = -sqrt(1.0 - lDHP);
		else
			hposnorm.z = .0;

		old_pos.handle = hposnorm;

		// Map tip position onto a hemisphere.  
		vec3 tip;
		tip.x = nauxpos.x * SENSITIVITY + old_pos.tip.x;
		tip.y = nauxpos.y * SENSITIVITY + old_pos.tip.y;
		double lD = tip.norm2();
		if (lD < 1.0)
			tip.z = -sqrt(1.0 - lD);
		else
			tip.z = .0;

		old_pos.tip = tip;
		if (old_pos.tip.x > 1) old_pos.tip.x = 1;
		if (old_pos.tip.x < -1) old_pos.tip.x = -1;
		if (old_pos.tip.y > 1) old_pos.tip.y = 1;
		if (old_pos.tip.y < -1) old_pos.tip.y = -1;
		if (old_pos.tip.z > 1) old_pos.tip.z = 1;
		if (old_pos.tip.z < -1) old_pos.tip.z = -1;
		if (old_pos.handle.x > 1) old_pos.handle.x = 1;
		if (old_pos.handle.x < -1) old_pos.handle.x = -1;
		if (old_pos.handle.y > 1) old_pos.handle.y = 1;
		if (old_pos.handle.y < -1) old_pos.handle.y = -1;
		if (old_pos.handle.z > 1) old_pos.handle.z = 1;
		if (old_pos.handle.z < -1) old_pos.handle.z = -1;

		tip *= SWORD_LENGTH + 1.0;

		vec3 swordvec = tip - hposnorm;
		swordvec = ~swordvec;

		swordvec *= SWORD_LENGTH;
		swordvec += hposnorm;

		ret.handle = hposnorm;
		ret.tip = swordvec;

		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += MEATSACK_HEIGHT * .25;
        ret.handle.z += MEATSACK_FATNESS * -1.0;
		ret.tip.z += MEATSACK_FATNESS * -1.0;

        return ret;

    }
};

// double hemispheres, one for tip pos and one for hilt pos.
// 
class TipCentricHemis : public Stance
{
public:
	TipCentricHemis() 
		:Stance("TipCentricHemis")
	{ max_force = 1100; }

	virtual Stance* Duplicate() const { return new TipCentricHemis; }

    Positions get_positions(vec2 pos) const
	{
		Positions ret;
		ret.handle = vec3(0, 0, 0);
		ret.tip = vec3(0, 0, 0);
		return ret;
	}

	Positions get_positions(const vec2 &pos, const vec2 &auxpos) const 
	{
		// Auxpos determines the wrist angle.  
		// Overall position comes from pos.

        Positions ret;

		// Map hilt position onto a hemisphere.  
		vec3 hposnorm;
		hposnorm.x = pos.x;
		hposnorm.y = pos.y;
		double lDHP = pos.norm2();
		if (lDHP < 1.0)
			hposnorm.z = -sqrt(1.0 - lDHP);
		else
			hposnorm.z = .0;

		// Map tip position onto a hemisphere.  
		vec3 tip;
		tip.x = auxpos.x;
		tip.y = auxpos.y;
		double lD = auxpos.norm2();
		if (lD < 1.0)
			tip.z = -sqrt(1.0 - lD);
		else
			tip.z = .0;

		tip *= SWORD_LENGTH + 1.0;

		vec3 swordvec = hposnorm - tip;
		swordvec = ~swordvec;

		swordvec *= SWORD_LENGTH;
		swordvec += tip;

		ret.handle = swordvec;
		ret.tip = tip;

		ret.handle.y += MEATSACK_HEIGHT * .25;
		ret.tip.y += MEATSACK_HEIGHT * .25;
        ret.handle.z += MEATSACK_FATNESS * -1.0;
		ret.tip.z += MEATSACK_FATNESS * -1.0;

        return ret;

    }
};



class DeadStance : public WristStance
{
public:
	DeadStance()
		:WristStance("DeadStance")
	{
		this->max_force = 10;
	}

	virtual Stance* Duplicate() const { return new DeadStance; }

};


class HorizontalBlockStance : public Stance
{
public:
	HorizontalBlockStance() 
		:Stance("HorizontalBlockStance"), direction_(HANDLE_LEFT) { }

	virtual Stance* Duplicate() const { return new HorizontalBlockStance; }

    void enter(vec3 pos, Quaternion rot, Sword* sword) {
        double left_dist  = (pos + rot.rotate(left_pos)  - sword->get_handle_position()).norm2()
                          + (pos + rot.rotate(right_pos) - sword->get_tip_position()).norm2();
        double right_dist = (pos + rot.rotate(left_pos)  - sword->get_tip_position()).norm2()
                          + (pos + rot.rotate(right_pos) - sword->get_handle_position()).norm2();
        if (left_dist < right_dist) {
            direction_ = HANDLE_LEFT;
        }
        else {
            direction_ = HANDLE_RIGHT;
        }
    }
    
    Positions get_positions(vec2 pos) const {
		pos *= 2.0;
        Positions ret;
        ret.handle = (direction_ == HANDLE_LEFT ? left_pos : right_pos) 
                       + vec3(pos.x/3, pos.y/3, 0);
        ret.tip    = (direction_ == HANDLE_LEFT ? right_pos : left_pos) 
                       + vec3(pos.x/3, pos.y/3, 0);
        return ret;
    }
private:
    static const vec3 left_pos;
    static const vec3 right_pos;
    
    enum {
        HANDLE_LEFT, HANDLE_RIGHT
    } direction_;
};

class VerticalBlockStance : public Stance
{
public:
	VerticalBlockStance() 
		:Stance("VerticalBlockStance")
	{}

	virtual Stance* Duplicate() const { return new VerticalBlockStance; }

    Positions get_positions(vec2 pos) const {
		pos *= 2.0;
        Positions ret;
        ret.handle = vec3(pos.x/3, 0.8 + pos.y/3, -0.3);
        ret.tip    = vec3(pos.x/3, 2   + pos.y/3, -0.3);
        return ret;
    }
};

#endif
