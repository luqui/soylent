#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "config.h"
#include "Actor.h"
#include "Texture.h"

class SoldierState : public ActorState
{
public:
    SoldierState(const tvec& ref, const vec& vel) 
        : ref_(ref), vel_(vel) 
    { }
    virtual ~SoldierState() { }

    virtual num gheight(const tvec&) const { return ZG_SOLDIER; }
    virtual num lheight(const tvec&) const { return ZL_SOLDIER; }
    virtual tvec ref() const { return ref_; }
    virtual vec vel() const { return vel_; }

private:
    tvec ref_;
    vec vel_;
};

class PikemanState : public SoldierState
{
public:
    PikemanState(const tvec& ref, const vec& vel);
    virtual ~PikemanState() { }

    virtual void render() const;
    
    virtual const PikemanState* handle(Message* msg, const tvec& rcvd) const;
private:
    Texture* tex_;
};

#endif
