#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "config.h"
#include <SDL_mixer.h>
#include <string>
#include <iostream>
using std::string;

const int ntracks = 3;
const char* tracks[] = { "dreamwalker" };

void music_finished_callback();

class Music {
	friend void music_finished_callback();
public:
	Music() : do_resume_(false),
			  intro_(false), free_(false), track_(0),
	          freemus_(0), intromus_(0), loopmus_(0) 
	{ }

	~Music() {
		if (freemus_)  Mix_FreeMusic(freemus_);
		if (intromus_) Mix_FreeMusic(intromus_);
		if (loopmus_)  Mix_FreeMusic(loopmus_);
		Mix_CloseAudio();
	}

	void init() {
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024);
		Mix_HookMusicFinished(&music_finished_callback);
	}

	void start() {
		load_track();
		intro_ = true;
		resume();
	}
	
	void load_track() {
		string track = string("media/") + tracks[track_];
		std::cout << "Loading track " << track << "\n";
		intromus_ = Mix_LoadMUS((track + "-intro.mp3").c_str());
		loopmus_  = Mix_LoadMUS((track + "-loop.mp3").c_str());
		std::cout << "  intro = " << intromus_ << "\n";
		std::cout << "  loop  = " << loopmus_ << "\n";
	}

	void resume() {
		if (freemus_) Mix_FreeMusic(freemus_);
		freemus_ = 0;
		if (intro_) {
			std::cout << "Playing intro\n";
			Mix_PlayMusic(intromus_, 1);
			intro_ = false;
		}
		else {
			std::cout << "Playing loop\n";
			Mix_PlayMusic(loopmus_, 1);
		}
	}

	void step() {
		if (do_resume_) {
			resume();
			do_resume_ = false;
		}
	}

	void advance() {
		if (intromus_) Mix_FreeMusic(intromus_);
		intromus_ = 0;

		freemus_ = loopmus_;
		
		std::cout << "Advancing from track " << track_ << " to track " << (track_ + 1) % ntracks << "\n";
		track_++;
		track_ %= ntracks;
		load_track();
		intro_ = true;
	}

private:
	bool do_resume_;
	bool intro_;
	bool free_;
	int track_;
	Mix_Music* freemus_;
	Mix_Music* intromus_;
	Mix_Music* loopmus_;
};

extern Music MUSIC;

inline void music_finished_callback() {
	std::cout << "Setting resume flag\n";
	MUSIC.do_resume_ = true;
}

#endif
