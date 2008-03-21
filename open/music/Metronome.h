#ifndef __METRONOME_H__
#define __METRONOME_H__

#include <iostream>
#include "physics.h"

template <class T>
bool update_q(T& ptr, T newval) {
	bool ret = false;
	if (newval != ptr) ret = true;
	ptr = newval;
	return ret;
}

struct Beat {
	Beat(double len, double offs = 0) : len(len), offs(offs) { }
	double len, offs;
};

double floatmod(double arg, double mod)
{
	if (arg < 0) {
		arg += (int(-arg/mod) + 1) * mod;
	}
	return fmod(arg, mod);
}

class Metronome 
{
public:
	Metronome(double tempo) 
		: stepsize_(DT * tempo / 60 / 4), measures_(0)
	{ }

	~Metronome() {}

	void step() {
		measures_ += stepsize_;
	}

	bool quantum(const Beat& beat) const {
		return floatmod(measures_ - beat.offs, beat.len) + stepsize_ >= beat.len;
	}

	double measures() const {
		return measures_;
	}

private:
	double stepsize_;
	double measures_;
};

#endif
