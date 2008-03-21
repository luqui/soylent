#ifndef __HIGHSCORESHEET_H__
#define __HIGHSCORESHEET_H__

#include "config.h"
#include "Utils.h"
#include <string>
#include <set>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

using std::string;
using std::set;

struct HighScoreEntry {
	HighScoreEntry(int score, string name, time_t date, unsigned int seed, int x, int y, float density) 
		: score(score), name(name), date(date), seed(seed), x(x), y(y), density(density)
	{ }
	
	int score;
	string name;
	time_t date;
	unsigned int seed;
	int x;
	int y;
	float density;

	bool operator < (const HighScoreEntry& o) const {
		return score <  o.score
			|| score == o.score && (
					date <  o.date ||
					date == o.date && (
						name < o.name));
	}	
};

class HighScoreSheet {
public:
	virtual ~HighScoreSheet() { }

	virtual string title() const = 0;
	virtual set<HighScoreEntry> entries() const = 0;
	
	virtual HighScoreEntry min_entry() const = 0;
	virtual void insert(const HighScoreEntry& e) = 0;
};

inline void ensure_dir_exists(string dir) 
{
#ifdef WIN32
	CreateDirectory(dir.c_str(), NULL);
#else
	mkdir(dir.c_str(), 0777);
#endif
}

class LocalHighScoreSheet : public HighScoreSheet {
public:
	LocalHighScoreSheet() : loaded_(false) { }
	
	string title() const {
		return "Local High Scores";
	}

	set<HighScoreEntry> entries() const {
		if (!loaded_) read_entries();
		return entries_;
	}

	HighScoreEntry min_entry() const {
		if (!loaded_) read_entries();
		if (entries_.size() < 10) return HighScoreEntry(999, "nobody", 0, 0, 0, 0, 0);
		set<HighScoreEntry>::iterator back = entries_.end();
		--back;
		return *back;
	}

	void insert(const HighScoreEntry& e) {
		if (!loaded_) read_entries();
		bool ret = false;
		if (entries_.size() < 10) {
			entries_.insert(e);
			ret = true;
		}
		else {
			set<HighScoreEntry>::iterator back = entries_.end();
			--back;
			if (back->score > e.score) {
				entries_.erase(back);
				entries_.insert(e);
				ret = true;
			}
		}
		if (ret) {
			write_entries();
		}
	}
private:
	void read_entries() const {
		SDL_RWops* file = SDL_RWFromFile(MINES_SCORESDIR "/scores.dat", "rb");
		loaded_ = true;
		if (!file) return;
		
		int nscores = 0;
		SDL_RWread(file, &nscores, sizeof(int), 1);
		for (int i = 0; i < nscores; i++) {
			int score = 0;
			SDL_RWread(file, &score, sizeof(int), 1);
			int nchars = 0;
			SDL_RWread(file, &nchars, sizeof(int), 1);
			char* name = new char[nchars+1];
			SDL_RWread(file, name, sizeof(char), nchars);
			name[nchars] = '\0';
			time_t date = 0;
			SDL_RWread(file, &date, sizeof(time_t), 1);
			unsigned int seed = 0;
			SDL_RWread(file, &seed, sizeof(unsigned int), 1);
			int x = 0;
			SDL_RWread(file, &x, sizeof(int), 1);
			int y = 0;
			SDL_RWread(file, &y, sizeof(int), 1);
			float density = 0;
			SDL_RWread(file, &density, sizeof(float), 1);
			entries_.insert(HighScoreEntry(score, string(name), date, seed, x, y, density));
			delete[] name;
		}
		SDL_RWclose(file);
	}

	void write_entries() const {
		ensure_dir_exists(MINES_SCORESDIR);
		SDL_RWops* file = SDL_RWFromFile(MINES_SCORESDIR "/scores.dat", "wb");
		if (!file) return;
		int nscores = int(entries_.size());
		SDL_RWwrite(file, &nscores, sizeof(int), 1);
		for (set<HighScoreEntry>::const_iterator i = entries_.begin(); i != entries_.end(); ++i) {
			SDL_RWwrite(file, &i->score, sizeof(int), 1);
			int nchars = i->name.size();
			SDL_RWwrite(file, &nchars, sizeof(int), 1);
			const char* buf = i->name.c_str();
			SDL_RWwrite(file, buf, sizeof(char), nchars);
			SDL_RWwrite(file, &i->date, sizeof(time_t), 1);
			SDL_RWwrite(file, &i->seed, sizeof(unsigned int), 1);
			SDL_RWwrite(file, &i->x, sizeof(int), 1);
			SDL_RWwrite(file, &i->y, sizeof(int), 1);
			SDL_RWwrite(file, &i->density, sizeof(float), 1);
		}
		SDL_RWclose(file);
	}
	
	// This object is being treated as an interface to the high score file;
	// i.e. const methods do not change the file.  That is why these
	// are mutable; we need to load data into the object, but that doesn't
	// change the file, so the load method is const.  I can't come up
	// with a reason why I wrote it this way instead of the conventional "const
	// doesn't change the object" way.
	
	mutable bool loaded_;
	mutable set<HighScoreEntry> entries_;
};

#endif
