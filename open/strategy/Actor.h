#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "config.h"
#include "vec.h"
#include "Widget.h"
#include "Messages.h"
#include "Relativity.h"
#include <deque>

// One actor state.  This should be one graphic (or animation)
// in an inertial reference frame (constant velocity).
class ActorState : public Widget
{
public:
    virtual ~ActorState() { }

    virtual tvec ref() const = 0;
    
    tvec tref(num time) const {    // return where I thought I was at time 'time'
        tvec r = ref();
        num dt = time - r.c;
        return r + tvec(dt, dt * vel());
    }
    
    virtual vec vel() const { return vec(); }

    virtual tvec tpos(const tvec& frame) const {
        return tref(light_line_to_point(ref(), vel(), frame));
    }

    vec pos(const tvec& ref) const {
        return tpos(ref).p;
    }

    // if handle returns non-null, then it returns a state change,
    // which is necessarily newer than the current state
    virtual const ActorState* handle(Message* msg, const tvec& ref) const = 0;

};

class ActorLog : public gc
{
public:
    void add_state(const ActorState* state);
    const ActorState* find_state(const tvec& ref) const;
    const ActorState* newest_state(num time) const;
    const ActorState* newest_state() const {
        if (states_.size()) { return *states_.rbegin(); }
        else                { return 0; }
    }
    

private:
    typedef std::deque< const ActorState*, gc_allocator< const ActorState* > > states_t;
    states_t states_;
};

class Actor : public gc
{
public:
    Actor(const ActorState* init, MessageTracker* tracker);
    void step();
    const ActorState* find_state(const tvec& ref) {
        return log_.find_state(ref);
    }

private:
    MessagePendingQueue messages_;
    ActorLog log_;
};


#endif
