#ifndef __HIGHSCORESHEET_H__
#define __HIGHSCORESHEET_H__

#include "tweak.h"
#include "util.h"
#include <string>
#include <set>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

using std::string;
using std::set;

struct HighScoreEntry {
	HighScoreEntry(int score, string name, time_t date, unsigned int seed) 
		: score(score), name(name), date(date), seed(seed)
	{ }
	
	int score;
	string name;
	time_t date;
	unsigned int seed;

	bool operator < (const HighScoreEntry& o) const {
		return score >  o.score
			|| score == o.score && (
					date <  o.date ||
					date == o.date && (
						name < o.name));
	}

	void erase_replay() const {
		remove(replay_name().c_str());
	}

	std::string replay_name() const {
		std::stringstream ss;
		ss << "scores/replay-" << seed << ".dat";
		return ss.str();
	}
		
};

class HighScoreSheet {
public:
	virtual ~HighScoreSheet() { }

	virtual string title() const = 0;
	virtual set<HighScoreEntry> entries() const = 0;
	virtual void draw(int selected = -1) const {
		set<HighScoreEntry> es = entries();
		int rh = H/2 + 4 * es.size();
		glColor3f(1,1,1);
		glRasterPos2f(65, rh);
		draw_string(title());
		rh -= 8;
		int idx = 0;
		for (set<HighScoreEntry>::iterator i = es.begin(); i != es.end(); ++i) {
			std::stringstream ss;
			char* date = ctime(&i->date);
			date[strlen(date)-1] = '\0';   // kill the \n terminator
			ss << std::setw(24) << std::left  << i->name << " "
			   << std::setw(10) << std::right << i->score << "     "
			   << std::setw(30) << std::left  << date;
			if (idx++ == selected) {
				glColor3f(1,1,0);
			}
			else {
				glColor3f(1,1,1);
			}
			glRasterPos2f(65, rh);
			draw_string(ss.str());
			rh -= 8;
		}
	}
	
	virtual HighScoreEntry min_entry() const = 0;
	virtual void insert(const HighScoreEntry& e) = 0;
};

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
		if (entries_.size() < 10) return HighScoreEntry(-1, "nobody", 0, 0);
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
			if (back->score < e.score) {
				back->erase_replay();
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
		SDL_RWops* file = SDL_RWFromFile(ICHOR_SCORESDIR "/scores.dat", "rb");
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
			entries_.insert(HighScoreEntry(score, string(name), date, seed));
			delete[] name;
		}
		SDL_RWclose(file);
	}

	void write_entries() const {
		SDL_RWops* file = SDL_RWFromFile(ICHOR_SCORESDIR "/scores.dat", "wb");
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
		}
		SDL_RWclose(file);
	}
	
	mutable bool loaded_;
	mutable set<HighScoreEntry> entries_;
};

#endif
