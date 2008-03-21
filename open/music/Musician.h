#ifndef __MUSICIAN_H__
#define __MUSICIAN_H__

#include "Synth.h"
#include <map>

class Technique
{
public:
	Technique(Metronome* metro, SynthTrack* synth) 
		: metro_(metro)
		, synth_(synth)
		, max_dur_add_(0)
	{ }

    // play a note for a duration
	void play_dur(int note, int vel, double dur) {
		synth_->note_on(note, vel);
		double now = metro_->measures();
		dur_notes_.insert(std::make_pair(now + dur, note));
	}
	
    // play a note for a maximum duration, or until another note is played
	void play_max_dur(int note, int vel, double dur) {
		double now = metro_->measures();
		if (max_dur_add_ != now) {
			for (notemap_t::iterator i = max_dur_notes_.begin(); i != max_dur_notes_.end(); ++i) {
				synth_->note_off(i->second);
			}
			max_dur_notes_.clear();
		}
		synth_->note_on(note, vel);
		max_dur_notes_.insert(std::make_pair(now + dur, note));
	}

	void step() {
		double now = metro_->measures();
		notemap_t::iterator upto = dur_notes_.upper_bound(now);
		for (notemap_t::iterator i = dur_notes_.begin(); i != upto; ++i) {
			synth_->note_off(i->second);
		}
		dur_notes_.erase(dur_notes_.begin(), upto);

		upto = max_dur_notes_.upper_bound(now);
		for (notemap_t::iterator i = max_dur_notes_.begin(); i != upto; ++i) {
			synth_->note_off(i->second);
		}
		max_dur_notes_.erase(max_dur_notes_.begin(), upto);
	}

private:
	typedef std::multimap<double, int> notemap_t;
	std::multimap<double, int> dur_notes_;
	std::multimap<double, int> max_dur_notes_;
	double max_dur_add_;
	
	Metronome* metro_;
	SynthTrack* synth_;
};

#endif
