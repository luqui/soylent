#ifndef __SOUND_H__
#define __SOUND_H__

#include <SDL_mixer.h>
#include <string>
#include "Musician.h"

class SoundEvent
{
public:
	SoundEvent(Technique* tech, int note, int vel, double dur) 
		: tech_(tech), note_(note), vel_(vel), dur_(dur)
	{ }
	
	void play() {
		tech_->play_dur(note_, vel_, dur_);
	}

	class Comparator {
	public:
		bool operator () (const SoundEvent* a, const SoundEvent* b) const {
			return a->tech_ <  b->tech_
				|| a->tech_ == b->tech_ && a->note_ < b->note_;
		}
	};

private:
	Technique* tech_;
	int note_, vel_;
	double dur_;
};

#endif
