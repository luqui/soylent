#ifndef __SOYLIB_FLOATER_H__
#define __SOYLIB_FLOATER_H__

#include <list>
#include "soy/weak.h"

class Floater {
public:
	virtual ~Floater() { }
	virtual void step() = 0;
	virtual bool done() = 0;
};

class FloaterList {
public:
	void add(weak<Floater> floater) {
		floaters_.push_back(floater);
	}

	void step() {
		for (floaters_t::iterator i = floater_.begin(); i != floater_.end(); ) {
			if (!i->valid() || (*i)->done()) {
				i->destroy();
				floaters_t::iterator j = i;
				++j;
				floaters_.erase(i);
				i = j;
			}
			else {
				(*i)->step();
			}
		}
	}
private:
	typedef std::list< weak<Floater> > floaters_t;
	floaters_t floaters_;
};

#endif
