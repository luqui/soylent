#ifndef __RATIONUNIT_H__
#define __RATIONUNIT_H__

#include "BUnit.h"
#include <list>

class RationUnit : public BUnit
{
public:
    // input:  0 = input
    // output: 0 = ration by time
    //         1 = overflow
    RationUnit() : BUnit(1,2), capacity_(0), rationtime_(0), timeleft_(0)
    { }

    void receive(BInterp* interp, int input, energy_t particle) {
        if (int(stored_.size()) < capacity_) {
            stored_.push_back(particle);
        }
        else {
            send(interp, 1, particle);
        }
    }

    void randomize() {
        capacity_ = rand() % RATION_MAX_INITIAL_CAPACITY;
        rationtime_ = randrange(0, RATION_MAX_INITIAL_TIME);
    }

    void mutate() {
        if (randrange(0,1) < RATION_CAPACITY_MUTATION_RATE) {
            capacity_ += rand() % 3 - 1;
            if (capacity_ < 0) { capacity_ = 0; }
        }
        if (randrange(0,1) < RATION_TIME_MUTATION_RATE) {
            rationtime_ += RATION_TIME_MUTATION * randrange(-1,1);
            if (rationtime_ < 0) { rationtime_ = 0; }
        }
    }

    void die(BInterp* interp) {
        while (!stored_.empty()) {
            interp->release(stored_.front());
            stored_.pop_front();
        }
    }

    void step(BInterp* interp) {
        timeleft_ -= DT;
        while (timeleft_ <= 0 && stored_.size() > 0) {
            send(interp, 0, stored_.front());
            stored_.pop_front();
            timeleft_ += rationtime_;
        }
    }

    RationUnit* clone() const {
        RationUnit* r = new RationUnit;
        copy_into(r);
        r->capacity_ = capacity_;
        r->rationtime_ = rationtime_;
        return r;
    }

    double size() const {
        return 0.5 + capacity_ / 10.0;
    }

    Color color() const {
        return Color(0.2, 0.8, 0.2);
    }
private:
    int capacity_;
    double rationtime_;

    std::list<energy_t> stored_;
    double timeleft_;
};

#endif
