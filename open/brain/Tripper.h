#ifndef __TRIPPER_H__
#define __TRIPPER_H__

#include <Trip.h>
#include <list>

class Tripper : public Trip {
public:
    ~Tripper() {
        for (trips_t::iterator i = trips_.begin(); i != trpis.end(); ++i) {
            delete *i;
        }
    }
    
    void add(Trip* t) {
        trips_.push_back(t);
    }

    bool done() const {
        return trips_.size() == 0;
    }

    void step() {
        trips_t::iterator i = trips_.begin();
        while (i != trips_.end()) {
            (*i)->step();
            trips_t::iterator tmp = i;
            ++i;
            if ((*i)->done()) {
                delete *tmp;
                trips_.erase(tmp);
            }
        }
    }

    void draw() const {
        for (trips_t::const_iterator i = trips_.begin(); i != trips_.end(); ++i) {
            (*i)->draw();
        }
    }
private:
    typedef std::list<Trip*> trips_t;
    trips_t trips_;
};

#endif
