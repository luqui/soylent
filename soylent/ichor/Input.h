#ifndef __INPUT_H__
#define __INPUT_H__

#include <SDL.h>
#include <list>

class Input
{
public:
	virtual ~Input() { }
	virtual vec get_direction() const = 0;
	virtual void step() = 0;
	virtual bool events(SDL_Event* e) = 0;
};

class WASDInput : public Input
{
public:
	vec get_direction() const { return dir_; }

	void step() {
		dir_ = vec();
		Uint8* keys = SDL_GetKeyState(NULL);
		if (keys[SDLK_a]) { dir_.x -= DT * FLOWSPEED; }
		if (keys[SDLK_d]) { dir_.x += DT * FLOWSPEED; }
		if (keys[SDLK_s]) { dir_.y -= DT * FLOWSPEED; }
		if (keys[SDLK_w]) { dir_.y += DT * FLOWSPEED; }
		if (dir_.norm() > FLOWSPEED * DT)
			dir_ = FLOWSPEED * DT * ~dir_;
	}

	bool events(SDL_Event* e) { return false; }
private:
	vec dir_;
};

class ArrowsInput : public Input
{
public:
	vec get_direction() const { return dir_; }
	
	void step() {
		dir_ = vec();
		Uint8* keys = SDL_GetKeyState(NULL);
		if (keys[SDLK_LEFT])  { dir_.x -= DT * FLOWSPEED; }
		if (keys[SDLK_RIGHT]) { dir_.x += DT * FLOWSPEED; }
		if (keys[SDLK_DOWN])  { dir_.y -= DT * FLOWSPEED; }
		if (keys[SDLK_UP])    { dir_.y += DT * FLOWSPEED; }
		if (dir_.norm() > FLOWSPEED * DT)
			dir_ = FLOWSPEED * DT * ~dir_;
	}
	
	bool events(SDL_Event* e) { return false; }
private:
	vec dir_;
};

class MouseInput : public Input
{
public:
	vec get_direction() const { return dir_; }

	void step() {
		// This copying is to ensure that the output only changes
		// once per frame, to secure replay integrity.
		dir_ = tmpdir_;
	}

	bool events(SDL_Event* e) {
		if (e->type == SDL_MOUSEMOTION) {
			vec dp(e->motion.xrel * 0.1, -e->motion.yrel * 0.1);
			tmpdir_ += dp;
			if (tmpdir_.norm() > FLOWSPEED * DT) {
				tmpdir_ = FLOWSPEED * DT * ~tmpdir_;
			}
			return true;
		}
		return false;
	}
private:
	vec tmpdir_;
	vec dir_;
};

class JoystickInput : public Input
{
public:
	vec get_direction() const { return dir_; }

	void step() {
		// See MouseInput::step for a description
		dir_ = tmpdir_;
	}

	bool events(SDL_Event* e) {
		if (e->type == SDL_JOYAXISMOTION) {
			if (e->jaxis.axis == 0) {
				tmpdir_.x = FLOWSPEED * DT * float(e->jaxis.value) / 32768;
				return true;
			}
			else if (e->jaxis.axis == 1) {
				tmpdir_.y = FLOWSPEED * DT * float(-e->jaxis.value) / 32768;
				return true;
			}
		}
		return false;
	}
private:
	vec tmpdir_;
	vec dir_;
};

class AddInput : public Input
{
	typedef std::list<Input*> inputs_t;
public:
	AddInput(inputs_t inputs) : inputs_(inputs) { }
	~AddInput() {
		for (inputs_t::iterator i = inputs_.begin(); i != inputs_.end(); ++i) {
			delete *i;
		}
	}
	
	vec get_direction() const { return dir_; }

	void step() {
		dir_ = vec();
		for (inputs_t::const_iterator i = inputs_.begin(); i != inputs_.end(); ++i) {
			(*i)->step();
			dir_ += (*i)->get_direction();
		}
		if (dir_.norm() > FLOWSPEED * DT) {
			dir_ = FLOWSPEED * DT * ~dir_;
		}
	}

	bool events(SDL_Event* e) {
		for (inputs_t::const_iterator i = inputs_.begin(); i != inputs_.end(); ++i) {
			if ((*i)->events(e)) return true;
		}
		return false;
	}
	
private:
	std::list<Input*> inputs_;
	vec dir_;
};

class WriterInput : public Input {
public:
	WriterInput(unsigned int seed, std::string file, Input* in) : in_(in), frames_(0) { 
		file_ = SDL_RWFromFile(file.c_str(), "wb");
		if (file_) {
			int writer = 0;
			SDL_RWwrite(file_, &seed, sizeof(unsigned int), 1);
			SDL_RWwrite(file_, &writer, sizeof(int), 1);
		}
	}

	~WriterInput() {
		if (file_) {
			SDL_RWseek(file_, sizeof(unsigned int), SEEK_SET);
			SDL_RWwrite(file_, &frames_, sizeof(int), 1);
			SDL_RWclose(file_);
		}
		delete in_;
	}

	vec get_direction() const { return dir_; }

	void step() {
		in_->step();
		dir_ = in_->get_direction();
		frames_++;
		if (file_) {
			SDL_RWwrite(file_, &dir_.x, sizeof(float), 1);
			SDL_RWwrite(file_, &dir_.y, sizeof(float), 1);
		}
	}

	bool events(SDL_Event* e) {
		return in_->events(e);
	}
private:
	Input* in_;
	int frames_;
	SDL_RWops* file_;
	vec dir_;
};

class ReaderInput : public Input
{
public:
	ReaderInput(std::string file) {
		file_ = SDL_RWFromFile(file.c_str(), "rb");
		if (file_) {
			SDL_RWread(file_, &seed_, sizeof(unsigned int), 1);
			SDL_RWread(file_, &frames_, sizeof(int), 1);
		}
		else {
			frames_ = 0;
			seed_ = 0;
		}
	}

	unsigned int get_seed() const { return seed_; }

	vec get_direction() const { return dir_; }

	void step() {
		if (file_) {
			if (frames_--) {
				SDL_RWread(file_, &dir_.x, sizeof(float), 1);
				SDL_RWread(file_, &dir_.y, sizeof(float), 1);
			}
			else {
				SDL_RWclose(file_);
				file_ = NULL;
				dir_ = vec();
			}
		}
		else {
			dir_ = vec();
		}
	}

	bool events(SDL_Event* e) { return false; }
private:
	SDL_RWops* file_;
	int frames_;
	unsigned int seed_;
	vec dir_;
};

#endif
