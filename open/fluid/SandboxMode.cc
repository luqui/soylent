#include <list>

#include "GameMode.h"
#include "Player.h"
#include "Explosion.h"
#include "util.h"
#include "Input.h"
#include "color.h"

class SandboxMode;

class Bacteria
{
public:
	Bacteria(SandboxMode* universe, vec pos, vec vel, Color color, Texture* tex, float size) : 
	  universe_(universe), pos_(pos), vel_(vel), color_(color), tex_(tex), size_(size) 
		{ }

	void step(GPUFluid* field);

	void draw() const {
		vec drawRad = vec(H/240.0, H/240.0);
		drawRad *= size_;
		color_.set_color_a(.5);
		TextureBinding b = tex_->bind();
		draw_rect(pos_ - drawRad, pos_ + drawRad);
	}

	bool dead() const {
		return size_ <= 0;
	}

private:
	SandboxMode* universe_;
	float size_;
	vec pos_;
	vec vel_;
	Color color_;
	Texture* tex_;

	Bacteria spawn_child() {
		size_ = 1;
		vec div = vec(rand()%100 - 50, rand()%100 - 50);
		float div_rate = 50;

		vec child_vel = vel_ - ~div * div_rate;
		vel_ += ~div * div_rate;
		
		return Bacteria(universe_, pos_, child_vel, color_, tex_, size_);
	}
};

class SandboxMode : public GameMode
{
public:
	SandboxMode(Input* input) : input_(input), field_(0) {
		init();
		bacteria_.push_back(Bacteria(this, vec(W/2, H/2), vec(0, 0), Color(0.7, 0.7, 0.7), load_texture(ICHOR_DATADIR "/bacteria.png"), 1));

		g_pFlo->EnableArbitraryBC(false);
	}
	
	~SandboxMode() {
		g_pFlo->EnableArbitraryBC(true);
		delete input_;
		delete field_;
		delete player_;
	}
	
	void step() {
		frame_input();
		check_bacteria();
		player_->step(field_);
		field_->step();
	}

	void draw() const {
		draw_board(field_, W, H);
		draw_bacteria();
		player_->draw();
	}

	bool events(SDL_Event* e) {
		return input_->events(e);
	}

	void add_bacteria(Bacteria spawn) {
		bacteria_.push_back(spawn);
	}

private:
	Sprayer* player_;
	GPUFluid* field_;
	Input* input_;

	std::list<Bacteria> bacteria_;

	void init() {
		field_ = new GPUFluid(W,H,vec(0,0), vec(W,H), DIFFUSION, VISCOSITY);
		player_ = new Sprayer(vec(W/10, H/2), load_texture(ICHOR_DATADIR "/redfirefly.png"));
	}

	void frame_input() {
		input_->step();
		player_->set_blow(input_->get_direction());
		player_->set_move(input_->get_movement());
	}

	void check_bacteria() {
		for (std::list<Bacteria>::iterator i = bacteria_.begin(); i != bacteria_.end();) {
			i->step(field_);
			if (i->dead()) {
				std::list<Bacteria>::iterator j = i;
				++i;
				bacteria_.erase(j);
			}
			else {
				++i;
			}
		}
	}

	void draw_bacteria() const {
		for (std::list<Bacteria>::const_iterator i = bacteria_.begin(); i != bacteria_.end(); i++) {
			i->draw();
		}
	}
};

void Bacteria::step(GPUFluid* field) {
		if (field->get_density(pos_) > 0) {
			float size_change = min(field->get_density(pos_), .1);
			size_ += size_change;
			field->add_density(pos_, -size_change*50, 1.0/float(H/2));
		}

		size_ -= 0.1*DT;
		if (size_ <= 0) return;
		if (size_ >= 5) {
			universe_->add_bacteria(spawn_child());
		}
		
		const float SEARCH = H / 24;
		vec grad;
		grad.x = (field->get_density(pos_ + vec(SEARCH,0)) - field->get_density(pos_ + vec(-SEARCH,0))) / 2.0;
		grad.y = (field->get_density(pos_ + vec(0,SEARCH)) - field->get_density(pos_ + vec(0,-SEARCH))) / 2.0;
		vec accel = 30 * ~grad;
		vel_ += accel * DT;

		vel_ += field->get_velocity(pos_)/(sqrt(size_));
		vel_ *= pow(float(0.214), float(DT));

		field->add_velocity(pos_ - H/40 * ~accel, -accel/5);

		pos_ += vel_ * DT;
		pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
		pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
	}

GameMode* make_SandboxMode(Input* inp) {
	return new SandboxMode(inp);
}