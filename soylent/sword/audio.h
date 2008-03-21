#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <string>
#include <map>
#include "config.h"
#include "SDL_mixer.h"

using std::string;
using std::map;

class Sound
{
public:
	Sound() : audio_available_(false) { }

	~Sound() {
		for (map<string, Mix_Chunk*>::iterator i = samples_.begin(); i != samples_.end(); i++) {
			Mix_FreeChunk(i->second);
		}
	}

	void play(string filename) {
        if (!audio_available_) return;

		if (samples_.find(filename) == samples_.end()) {
			Mix_Chunk* sample = Mix_LoadWAV((SOUNDDIR + filename).c_str());
			if (!sample) {
				DIE(Mix_GetError());
			}
			samples_[filename] = sample;
		}
		Mix_PlayChannel(-1, samples_[filename], 0);
	}

	void init_audio() {
		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == 0) {
            audio_available_ = true;
        }
        else {
            audio_available_ = false;
            std::cerr << "Failed to open audio device: " << Mix_GetError() << "\n";
		}
	}

private:
    bool audio_available_;
	map<string, Mix_Chunk*> samples_;
};

#endif
