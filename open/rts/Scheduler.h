#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <list>
#include <soy/Timer.h>

class Computation {
public:
	virtual ~Computation() { }
	virtual bool compute(num timeslice) = 0;  // returns whether it finished
};

class Scheduler : public Computation {
public:
	~Scheduler() {
		for (pending_t::iterator i = pending_.begin(); i != pending_.end(); ++i) {
			delete *i;
		}
	}

	bool compute(num timeslice) {
		Timer timer;
		timer.init();
		
		while (pending_.size()) {
			Computation* cur = pending_.front();
			bool finished = cur->compute(timeslice);
			if (!finished) {  // if the algorithm had no more time, neither do we
				return false;
			}

			delete cur;
			pending_.pop_front();
			timeslice -= timer.get_time_diff();
			if (timeslice <= 0) break;
		}
		return true;
	}

	void schedule(Computation* comp) {
		pending_.push_back(comp);
	}

	void cancel(Computation* comp) {
		for (pending_t::iterator i = pending_.begin(); i != pending_.end(); ++i) {
			if (*i == comp) {
				delete *i;
				pending_.erase(i);
				return;
			}
		}
	}

private:
	typedef std::list<Computation*> pending_t;
	pending_t pending_;
};

#endif
