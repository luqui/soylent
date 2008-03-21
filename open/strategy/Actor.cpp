#include "Actor.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>

struct RefClockSorter
{
    bool operator() (const ActorState* a, const ActorState* b) {
        return a->ref().c < b->ref().c;
    }
};

void ActorLog::add_state(const ActorState* state) {
    states_.push_back(state);
    states_t::iterator preend = states_.end(); --preend;
    std::inplace_merge(states_.begin(), preend, states_.end(), RefClockSorter());
}

const ActorState* ActorLog::find_state(const tvec& ref) const {
    if (states_.size() == 0) return 0;
    // These checks use the "relativistic intermediate value theorem"
    // which assumes nothing goes faster than the speed of light.
    
    // If the earliest interval is spacelike, then we can never go timelike
    if ((states_[0]->ref() - ref).norm2() < 0) return 0;

    // If the latest interval is timelike and not in the future,
    // then we'll use that.
    tvec lastref = states_[states_.size() - 1]->ref();
    if (lastref.c <= ref.c && (lastref - ref).norm2() >= 0)
        return states_[states_.size()-1];

    // lo is the best timelike interval we've found
    // hi is the best spacelike interval we've found
    int lo = 0, hi = states_.size() - 1;
    while (hi - lo > 1) {
        int mid = (hi + lo) / 2;
        if (states_[mid]->ref().c > ref.c) {  // Rule out events in the future
            hi = mid; continue;
        }
        num interval = (states_[mid]->ref() - ref).norm2();
        if      (interval > 0) lo = mid;
        else if (interval < 0) hi = mid;
        else if (interval == 0) // yeah right
                return states_[mid];
    }
    return states_[lo];
}

const ActorState* ActorLog::newest_state(num time) const {
    if (states_.size() == 0) return 0;
    if (states_[0]->ref().c > time) return 0;
    if (states_.back()->ref().c <= time) return states_.back();

    int lo = 0, hi = states_.size() - 1;
    while (hi - lo > 1) {
        int mid = (hi + lo) / 2;
        if      (states_[mid]->ref().c < time) lo = mid;
        else if (states_[mid]->ref().c > time) hi = mid;
        else if (states_[mid]->ref().c == time) return states_[mid];
    }
    return states_[lo];
}



Actor::Actor(const ActorState* init, MessageTracker* tracker)
    : messages_(tracker)
{
    log_.add_state(init);
}

void Actor::step()
{
    const ActorState* top = log_.newest_state(CLOCK);
    tvec ref = top->tref(CLOCK);
    while (Message* msg = messages_.poll(ref)) {
        if (const ActorState* newstate = top->handle(msg, ref)) {
            log_.add_state(newstate);
        }
    }
}
