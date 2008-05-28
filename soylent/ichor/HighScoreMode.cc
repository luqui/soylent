#include "GameMode.h"
#include "HighScoreSheet.h"
#include "Input.h"

class HighScoreMode : public GameMode
{
public:
	HighScoreMode(HighScoreSheet* sheet) : sheet_(sheet), index_(0), mode_(0) { }

	~HighScoreMode() {
		delete mode_;
		delete sheet_;
	}

	void step() {
		if (mode_) {
			if (QUIT) {
				delete mode_;
				mode_ = 0;
				QUIT = false;
			}
			else {
				mode_->step();
			}
		}
	}

	void draw() const {
		if (mode_) {
			mode_->draw();
		}
		else {
			sheet_->draw(index_);
		}
	}

	bool events(SDL_Event* e) {
		if (mode_) return mode_->events(e);

		if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_RETURN || e->key.keysym.sym == SDLK_SPACE) {
				std::set<HighScoreEntry> entries = sheet_->entries();
				std::set<HighScoreEntry>::const_iterator i = entries.begin();
				for (int j = 0; j < index_; j++) {
					++i;
				}
				mode_ = make_GalagaMode(new ReaderInput(i->replay_name()));
				return true;
			}
			else if (e->key.keysym.sym == SDLK_DOWN) {
				index_++;
				while (index_ >= int(sheet_->entries().size())) {
					index_ -= sheet_->entries().size();
				}
				return true;
			}
			else if (e->key.keysym.sym == SDLK_UP) {
				index_--;
				while (index_ < 0) {
					index_ += sheet_->entries().size();
				}
				return true;
			}
		}
		return false;
	}
private:
	HighScoreSheet* sheet_;
	int index_;
	GameMode* mode_;
};

GameMode* make_HighScoreMode()
{
	return new HighScoreMode(new LocalHighScoreSheet);
}
