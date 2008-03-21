#ifndef __Jukebox_h__
#define __JukeBox_h__

#include <SDL_mixer.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>

void music_finished();

class Jukebox
{
	typedef std::vector<string> strlist_t;
public:
	void add_song(string songname) {
		songlist_.push_back(songname);
		std::random_shuffle(songlist_.begin(), songlist_.end());
		track_ = songlist_.begin();
	}

	void play() {
		string name = next_track();
		if (name == "") {
			return;
		}

		song = Mix_LoadMUS(name.c_str());
		if (!song) {
			std::cerr<<Mix_GetError()<<endl;
		}

		Mix_PlayMusic(song, 1);
		Mix_HookMusicFinished(&::music_finished);
	}

	void pause() {
		Mix_PauseMusic();
	}

	void resume() {
		Mix_ResumeMusic();
	}

	void stop() {
		Mix_HaltMusic();
		Mix_FreeMusic(song);
	}

	void music_finished() {
		Mix_FreeMusic(song);
		play();
	}


	~Jukebox() {
		stop();
	}

private:
	string next_track() {
		if (songlist_.size() < 1) {
			return "";
		}

		string ret = *track_;
		if (track_ != songlist_.end()) {
			track_++;
		}
		else {
			std::random_shuffle(songlist_.begin(), songlist_.end());
			track_ = songlist_.begin();
		}

		return ret;
	}
	
	Mix_Music* song;
	strlist_t songlist_;
	strlist_t::iterator track_;
};

extern Jukebox* JUKEBOX;

void music_finished() {
	JUKEBOX->music_finished();
}

#endif