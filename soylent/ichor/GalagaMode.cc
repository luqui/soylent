#include "GameMode.h"
#include "Player.h"
#include "Explosion.h"
#include "Texture.h"
#include "Input.h"
#include <list>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "HighScoreSheet.h"

class GalagaMode : public GameMode
{
private:
	Player* player_;

	FluidDensityGrid* field_;
	
	class Joint;
	class Enemy;
	class Level;

	std::list<Enemy*> enemies_;
	std::list<Explosion*> explosions_;
	std::list<Joint> joints_;

	HighScoreSheet* scores_;
	
	float invincible_time_;
	int score_;
	float level_delay_;
	bool level_complete_;
	bool dead_;
	int combo_;
	float combo_time_;
	
	unsigned int seed_;
	Input* input_;
	bool record_;
	std::string log_file_;
	bool high_score_;

	float level_display_time_;
	int level_num_;
	Level* level_;
	
	void frame_input() {
		input_->step();
		player_->set_blow(input_->get_direction());
	}

	void draw_score() const {
		std::stringstream ss;
		ss << score_;
		std::string text = ss.str();
		
		glColor3f(1,1,1);
		glRasterPos2f(W-21, H-3);
		for (size_t i = 0; i < text.size(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
		}
	}

	void display_high_scores() const {
		scores_->draw();
	}

	void load_high_scores() {
		scores_ = new LocalHighScoreSheet;
		if (record_) {
#ifdef WIN32
			HighScoreEntry entry(score_, getenv("USERNAME"), time(NULL), seed_);
#else
			HighScoreEntry entry(score_, getenv("USER"), time(NULL), seed_);
#endif
			if (entry < scores_->min_entry()) {  // '<' means '>' here
				scores_->insert(entry);
				high_score_ = true;
			}
		}
	}

	int compute_score(int par_score, float par_time, float lifetime) {
		return combo_ * par_score;
	}

	float spawn_position_weight(vec p) const {
		float dens = field_->get_density(p);
		float playerdist = (p - player_->position()).norm();
		float enemydist = HUGE_VAL;
		for (std::list<Enemy*>::const_iterator i = enemies_.begin(); i != enemies_.end(); ++i) {
			float dist = ((*i)->position() - p).norm();
			if (dist < enemydist) enemydist = dist;
		}
		
		float weight = 0;
		// points for being bright in color
		if (dens < 1/100.0) weight += 1 - 100*dens;
		// not allowed to be within 50 units of the player
		if (playerdist < 50) return 0;
		// points for being far away from the player
		weight += playerdist/100;
		// points for being far away from any other enemies
		if (enemydist < HUGE_VAL) weight += enemydist/200;
		return weight;
	}

	vec generate_spawn_position() const {
		vec best;
		float sum = 0;
		for (int x = CLAMPW; x < W - CLAMPW; x++) {
			for (int y = CLAMPH; y < H - CLAMPH; y++) {
				float weight = spawn_position_weight(vec(x,y));
				sum += weight;
				if (randrange(0, sum) < weight) {
					best = vec(x,y);
				}
			}
		}
		return best;
	}

	void prune_joints(Enemy* enemy) {
		for (std::list<Joint>::iterator i = joints_.begin(); i != joints_.end(); ) {
			if (i->a == enemy || i->b == enemy) {
				std::list<Joint>::iterator j = i;
				++j;
				joints_.erase(i);
				i = j;
			}
			else {
				++i;
			}
		}
	}

	bool joint_exists(Enemy* a, Enemy* b) const {
		for (std::list<Joint>::const_iterator i = joints_.begin(); i != joints_.end(); ++i) {
			if (i->a == a && i->b == b || i->a == b && i->b == a) {
				return true;
			}
		}
		return false;
	}

	void init() {
		player_ = new Player(Color(1, 0.5, 0), 1, vec(W/2, H/2), 1, load_texture(ICHOR_DATADIR "/redfirefly.png"));
		field_ = new FluidDensityGrid(W, H, vec(0,0), vec(W,H), DIFFUSION, VISCOSITY);
		scores_ = NULL;
		invincible_time_ = 0;
		score_ = 0;
		level_delay_ = 0;
		level_complete_ = false;
		dead_ = false;
		combo_ = 1;
		combo_time_ = 0;
		high_score_ = false;
		level_num_ = 1;
		level_ = make_level(level_num_);
		level_display_time_ = 3;
		explosions_.push_back(new DensityGlobExplosion(vec(W/2,H/2), 0.2, 2.5, 15, 5));
		init_color();
	}

	void draw_level() const {
		if (level_) {
			glColor3f(1,1,1);
			glRasterPos2f(W/2-10, H/3);
			draw_string(level_->title());
		}
	}

	void spawn_animation(vec pos) {
		float balance = field_->get_balance();
		float power = -2.5 - 2.5*balance/100;
		if (power > 0) power = 0;
		explosions_.push_back(new DensityGlobExplosion(pos, 0.2, power, 15, 5));
	}

	template <class E>
	void spawn_enemy(float diff = 1) {
		vec pos = generate_spawn_position();
		spawn_animation(pos);
		enemies_.push_back(new E(pos, 0.3, diff));
	}

	void maybe_advance_level() {
		if (!level_complete_) {
			if (level_ && level_->advance()) {
				level_complete_ = true;
				level_delay_ = 0.5;
			}
		}
		else if (level_delay_ <= 0) {
			delete level_;
			level_num_++;
			level_ = make_level(level_num_);
			if (level_ == NULL) {
				load_high_scores();
			}
			level_complete_ = false;
		}
	}
	
public:
	GalagaMode(Input* input, unsigned int seed) 
		: seed_(seed), input_(input), record_(true)
	{
		srand(seed_);
		log_file_ = HighScoreEntry(0, "nobody", 0, seed_).replay_name();
		input_ = new WriterInput(seed_, log_file_, input_);
		init();
	}

	GalagaMode(ReaderInput* input)
		: input_(input), record_(false)
	{
		seed_ = input->get_seed();
		srand(seed_);
		init();
	}
	
	~GalagaMode() {
		delete player_;
		delete input_;
		if (record_ && !high_score_) {
			remove(log_file_.c_str());
		}
		delete field_;
		delete scores_;
		for (std::list<Enemy*>::iterator i = enemies_.begin(); i != enemies_.end(); ++i) {
			delete *i;
		}
		for (std::list<Explosion*>::iterator i = explosions_.begin(); i != explosions_.end(); ++i) {
			delete *i;
		}
	}
	
	void step() {
		frame_input();

		field_->step_velocity();
		field_->step_density();

		if (!dead_) player_->step(field_);

		invincible_time_ -= DT;
		level_display_time_ -= DT;
		if (!dead_ && invincible_time_ <= 0 && field_->get_density(player_->position()) < -DEATH_DENSITY) {
			player_->die();
			if (player_->get_life() <= 0) {
				dead_ = true;
			}
			invincible_time_ = 3;
			explosions_.push_back(new InwardVortexExplosion(player_->get_color(), player_->position(), 1, 3, W));
		}
		if (invincible_time_ <= 2 && dead_ && !scores_) {
			load_high_scores();
		}

		combo_time_ -= DT;
		for (std::list<Enemy*>::iterator i = enemies_.begin(); i != enemies_.end();) {
			if ((*i)->dead(this)) {
				score_ += compute_score((*i)->par_score(), (*i)->par_time(), (*i)->lifetime());
				if (combo_time_ > 0) { combo_++; }
				else                 { combo_ = 1; }
				combo_time_ = 1.5;
				if ((*i)->should_explode()) {
					if (combo_ < 4) {
						explosions_.push_back(new DensityGlobExplosion((*i)->position(), sqrt(float(combo_))*0.2, combo_*5, sqrt(float(combo_))*15, combo_*5));
					}
					else {
						explosions_.push_back(new InwardVortexExplosion(Color(1, 1, 1), (*i)->position(), 2, 0.833, 60));
					}
				}
				prune_joints(*i);
				delete *i;
				std::list<Enemy*>::iterator j = i;
				++j;
				enemies_.erase(i);
				i = j;
				continue;
			}

			for (std::list<Enemy*>::iterator j = enemies_.begin(); j != enemies_.end(); ++j) {
				if (i == j) continue;
				if (((*i)->position() - (*j)->position()).norm() < 8) {
					if (!joint_exists(*i, *j)) {
						Joint joint(*i, *j);
						if ((*i)->join(*j, &joint, this) && (*j)->join(*i, &joint, this)) {
							joints_.push_back(joint);
						}
					}
				}
			}
			
			(*i)->step(this);
			++i;
		}

		for (std::list<Joint>::iterator i = joints_.begin(); i != joints_.end(); ++i) {
			i->step();
		}

		for (std::list<Explosion*>::iterator i = explosions_.begin(); i != explosions_.end();) {
			if ((*i)->done()) {
				delete *i;
				std::list<Explosion*>::iterator j = i;
				++j;
				explosions_.erase(i);
				i = j;
			}
			else {
				(*i)->step(field_);
				++i;
			}
		}

		level_delay_ -= DT;
		if (!dead_ && level_) {
			level_->step();
		}
		maybe_advance_level();
	}

	void draw() const {
		draw_board(field_, W, H);
		for (std::list<Enemy*>::const_iterator i = enemies_.begin(); i != enemies_.end(); ++i) {
			(*i)->draw(this);
		}
		for (std::list<Explosion*>::const_iterator i = explosions_.begin(); i != explosions_.end(); ++i) {
			(*i)->draw();
		}
		if (!dead_) {
			player_->draw();
			player_->draw_lives(vec(3,H-3), 1);
		}
		draw_score();
		if (scores_) {
			display_high_scores();
		}
		if (level_display_time_ > 0) {
			draw_level();
		}
	}

	bool events(SDL_Event* e) {
		if (scores_) {
			if (e->type == SDL_KEYDOWN || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_JOYBUTTONDOWN) {
				QUIT = true;
				return true;
			}
		}
		return input_->events(e);
	}

private:
	class Level {
	public:
		virtual void step() = 0;
		virtual bool advance() const = 0;
		virtual ~Level() {}
		virtual std::string title() const {return "";}
	};

	class Wave: public Level {
	public:
		Wave(GalagaMode* mode): spawned_(false), mode_(mode) { }
		void step() {
			if (!spawned_) {
				spawn();
				spawned_ = true;
			}
		}

		bool advance() const {
			return mode_->enemies_.size() == 0;
		}

		virtual void spawn() = 0;

	protected:
		bool spawned_;
		GalagaMode* mode_;
	};

	class RandomEnemies: public Level {
	public:
		RandomEnemies(GalagaMode* mode, int enemies_left, float spawn_rate): 
		  mode_(mode), enemies_left_(enemies_left), spawn_rate_(spawn_rate), time_left_(0) { }

		bool advance() const {
			return enemies_left_ <= 0 && mode_->enemies_.size() == 0;
		}

		void step() {
			time_left_ -= DT;
			if (enemies_left_ > 0 && (time_left_ <= 0 || mode_->enemies_.size() == 0)) {
				spawn();
				enemies_left_--;
				time_left_ = spawn_rate_;
			}
		}

		virtual void spawn() = 0;

	protected:
		int enemies_left_;
		float spawn_rate_;
		float time_left_;
		GalagaMode* mode_;
	};

#define MAKE_RANDOM_SERIES(NAME, ENEMIES, RATE, CODE) \
	class NAME : public RandomEnemies { \
	public: \
		NAME(GalagaMode* mode) : RandomEnemies(mode, ENEMIES, RATE) { } \
		void spawn() CODE \
	};

#define MAKE_WAVE(NAME, CODE) \
	class NAME : public Wave { \
	public: \
		NAME(GalagaMode* mode) : Wave(mode) { } \
		void spawn() CODE \
	};

	MAKE_WAVE(Level1, {
		mode_->spawn_enemy<BasicEnemy>();
	});

	MAKE_RANDOM_SERIES(Level2, 6, 3.5, {
		mode_->spawn_enemy<BasicEnemy>();
	});

	MAKE_WAVE(Level3, {
		mode_->spawn_enemy<SpinEnemy>();
	});

	MAKE_WAVE(Level4, {
		mode_->spawn_enemy<MissileEnemy>();
	});

	MAKE_RANDOM_SERIES(Level5, 15, 3, {
		switch (rand()% 3) {
		case 0:
			mode_->spawn_enemy<BasicEnemy>();
			break;
		case 1:
			mode_->spawn_enemy<MissileEnemy>();
			break;
		case 2:
			mode_->spawn_enemy<SpinEnemy>();
			break;
		}
	});

	MAKE_WAVE(Level6, {
		mode_->spawn_enemy<BasicEnemy>();
		mode_->spawn_PairBomb_pair(mode_->generate_spawn_position());
	});

	MAKE_WAVE(Level7, {
		mode_->spawn_enemy<ScorpionEnemy>();
		mode_->spawn_enemy<MissileEnemy>();
	});

	MAKE_WAVE(Level8, {
		mode_->spawn_enemy<VortexEnemy>();
	});

	MAKE_WAVE(Level9, {
		mode_->spawn_enemy<BasicEnemy>();
		mode_->spawn_enemy<BasicEnemy>();
		mode_->spawn_enemy<BasicEnemy>();
		mode_->spawn_enemy<BasicEnemy>();
		mode_->spawn_enemy<BasicEnemy>();
	});
	
	MAKE_WAVE(Level10, {
		mode_->spawn_enemy<MissileEnemy>();
		mode_->spawn_enemy<MissileEnemy>();
		mode_->spawn_enemy<MissileEnemy>();
		mode_->spawn_enemy<SpinEnemy>();
	});
	
	MAKE_RANDOM_SERIES(Level11, 100, 3, {
		if (enemies_left_ % 20 == 0) {
			mode_->spawn_enemy<VortexEnemy>();
			return;
		}
		switch (rand()% 5) {
		case 0:
			mode_->spawn_enemy<BasicEnemy>();
			break;
		case 1:
			mode_->spawn_enemy<MissileEnemy>();
			break;
		case 2:
			mode_->spawn_enemy<SpinEnemy>();
			break;
		case 3:
			mode_->spawn_PairBomb_pair(mode_->generate_spawn_position());
			break;
		case 4:
			mode_->spawn_enemy<ScorpionEnemy>();
			break;
		}
	});

	Level* make_level(int num) {
		switch (num) {
			case 1:  return new Level1(this);
			case 2:  return new Level2(this);
			case 3:  return new Level3(this);
			case 4:  return new Level4(this);
			case 5:  return new Level5(this);
			case 6:  return new Level6(this);
			case 7:  return new Level7(this);
			case 8:  return new Level8(this);
			case 9:  return new Level9(this);
			case 10: return new Level10(this);
			case 11: return new Level11(this);

			default: return 0;
		}
	}
	
	class Enemy {
	public:
		virtual ~Enemy() { }

		virtual void draw(const GalagaMode* mode) = 0;
		virtual void step(GalagaMode* mode) = 0;
		virtual vec position() const = 0;
		virtual void set_position(vec pos) = 0;
		virtual bool vulnerable() const { return true; }
		virtual bool dead(const GalagaMode* mode) const {
			return vulnerable() && mode->field_->get_density(position()) > DEATH_DENSITY;
		}
		virtual bool should_explode() const { return true; }
		virtual int par_score() const { return 10; }
		virtual float par_time() const { return 5; }
		virtual float lifetime() const = 0;
		// returns whether to make the joint
		virtual bool join(Enemy* enemy, Joint* joint, GalagaMode* mode) { return true; }
	};

	class Joint
	{
	public:
		Joint(Enemy* a, Enemy* b) 
			: a(a), b(b), 
		      length((b->position() - a->position()).norm())
		{ }

		void step() {
			vec center = (a->position() + b->position())/2;
			a->set_position(center + length/2 * ~(a->position() - center));
			b->set_position(center + length/2 * ~(b->position() - center));
		}

		Enemy* a;
		Enemy* b;
		float length;
	};

	class BasicEnemy : public Enemy
	{
	public:
		BasicEnemy(vec pos, float wait, float difficulty) 
			: pos_(pos), wait_(wait), tex_(load_texture(ICHOR_DATADIR "/metalball.png")),
			  lifetime_(0), difficulty_(difficulty)
		{ }

		void draw(const GalagaMode* mode) {
			if (wait_ > 0) return;
			glColor3f(1,1,1);
			TextureBinding b = tex_->bind();
			draw_rect(pos_ - vec(4,4), pos_ + vec(4,4));
		}

		void step(GalagaMode* mode) {
			wait_ -= DT;
			if (wait_ > 0) return;
			lifetime_ += DT;
			pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
			pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
			pos_ += mode->field_->get_velocity(pos_) * 70 * DT;
			vec dir = ~(mode->player_->position() - pos_);
			mode->field_->add_density(pos_, -difficulty_ * 0.6);
			mode->field_->add_velocity(pos_ - 5*dir, 30*dir);
		}

		vec position() const { return pos_; }
		void set_position(vec pos) { pos_ = pos; }

		bool vulnerable() const { return wait_ <= 0; }
		float lifetime() const { return lifetime_; }
		int par_score() const { return 5; }

	private:
		vec pos_;
		float wait_;
		Texture* tex_;
		float lifetime_;
		float difficulty_;
	};

	class MissileEnemy : public Enemy
	{
	public:
		MissileEnemy(vec pos, float wait, float difficulty) 
			: pos_(pos), wait_(wait), theta_(0),
			  tex_(load_texture(ICHOR_DATADIR "/rocketball.png")),
			  inited_(false), lifetime_(0)
		{ }

		void draw(const GalagaMode* mode) {
			if (wait_ > 0) return;
			glPushMatrix();
				glTranslatef(pos_.x, pos_.y, 0);
				glRotatef(180 / M_PI * theta_, 0, 0, 1);
				glColor3f(1,1,1);
				TextureBinding b = tex_->bind();
				draw_rect(-vec(4,4), vec(4,4));
			glPopMatrix();
		}

		void step(GalagaMode* mode) {
			wait_ -= DT;
			if (wait_ > 0) return;
			lifetime_ += DT;
			pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
			pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
			
			vec playerdir = mode->player_->position() - pos_;
			float target_theta = atan2(playerdir.y, playerdir.x);
			if (!inited_) {
				theta_ = target_theta;
				inited_ = true;
			}

			if (fabs(theta_ + 2*M_PI - target_theta) < fabs(theta_ - target_theta)) {
				theta_ += 2*M_PI;
			}
			else if (fabs(theta_ - 2*M_PI - target_theta) < fabs(theta_ - target_theta)) {
				theta_ -= 2*M_PI;
			}
			if (fabs(theta_ - target_theta) > 1.75*DT) {
				theta_ += 1.75*DT * (target_theta - theta_ < 0 ? -1 : 1);
			}
			else {
				theta_ = target_theta;
			}

			vec dir(cos(theta_), sin(theta_));
			
			mode->field_->add_density(pos_, -.6);
			add_wide_velocity(mode->field_, pos_, 60*dir);
			pos_ += mode->field_->get_velocity(pos_) * 30 * DT;
		}

		vec position() const { return pos_; }
		void set_position(vec pos) { pos_ = pos; }
		bool vulnerable() const { return wait_ <= 0; }
		float lifetime() const { return lifetime_; }

	private:
		vec pos_;
		float wait_;
		float theta_;
		Texture* tex_;
		bool inited_;
		float lifetime_;
	};

	class ScorpionEnemy : public Enemy
	{
	public:
		ScorpionEnemy(vec pos, float wait, float difficulty) 
			: pos_(pos), wait_(wait), tex_(load_texture(ICHOR_DATADIR "/hornball.png")),
			  lifetime_(0)
		{ }

		void draw(const GalagaMode* mode) {
			if (wait_ > 0) return;
			vec dir = ~(mode->player_->position() - pos_);
			float angle = atan2(dir.y, dir.x)-M_PI/2;
			glPushMatrix();
				glTranslatef(pos_.x, pos_.y, 0);
				glRotatef(180/M_PI*angle, 0, 0, 1);
				glColor3f(1,1,1);
				TextureBinding b = tex_->bind();
				draw_rect(-vec(4,4), vec(4,4));
			glPopMatrix();
		}

		void step(GalagaMode* mode) {
			wait_ -= DT;
			if (wait_ > 0) return;
			lifetime_ += DT;
			pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
			pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
			vec grad(
				(mode->field_->get_density(pos_ + vec(10,0)) - mode->field_->get_density(pos_ + vec(-10,0))) / 2.0,
				(mode->field_->get_density(pos_ + vec(0,10)) - mode->field_->get_density(pos_ + vec(0,-10))) / 2.0);
			vec dir = ~(mode->player_->position() - pos_);
			mode->field_->add_density(pos_ + 5*dir, -0.6);
			mode->field_->add_velocity(pos_, 30*dir);
			pos_ -= 5000 * grad * DT;
		}

		vec position() const { return pos_; }
		void set_position(vec pos) { pos_ = pos; }
		bool vulnerable() const { return wait_ <= 0; }
		float lifetime() const { return lifetime_; }
		int par_score() const { return 20; }
	private:
		vec pos_;
		float wait_;
		Texture* tex_;
		float lifetime_;
	};

	class PairBomb : public Enemy
	{
	public:
		PairBomb(vec pos, float wait, float difficulty) 
			: wait_(wait), pos_(pos), other_(NULL), detonated_(false),
			  lifetime_(0)
		{ }

		~PairBomb() {
			if (other_) other_->other_ = NULL;
		}

		void set_other(PairBomb* other) {
			other_ = other;
		}
		
		void draw(const GalagaMode* mode) {
			if (wait_ > 0) return;
			glColor3f(1,1,0.3);
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for (int i = 0; i < 16; i++) {
				float theta = randrange(0, 2*M_PI);
				glVertex2f(pos_.x + 3*cos(theta), pos_.y + 3*sin(theta));
			}
			glEnd();
		}

		bool join(Enemy* enemy, Joint* joint, GalagaMode* mode) {
			if (enemy == other_) {
				mode->explosions_.push_back(new InwardVortexExplosion(Color(1, 1, 1), pos_, -2, 0.833, 60));
				detonated_ = true;
				other_->other_ = NULL;
				other_ = NULL;
				return false;
			}
			else {
				return true;
			}
		}

		void step(GalagaMode* mode) {
			wait_ -= DT;
			if (wait_ > 0 || other_ == NULL) return;
			lifetime_ += DT;
			vec dir = ~(other_->pos_ - pos_);
			add_wide_velocity(mode->field_, pos_, 15*dir);
			pos_ += mode->field_->get_velocity(pos_) * 40 * DT;
		}
		
		vec position() const { return pos_; }
		void set_position(vec pos) { pos_ = pos; }
		bool vulnerable() const { return wait_ <= 0; }
		bool dead(const GalagaMode* mode) const {
			return detonated_ || other_ == NULL || Enemy::dead(mode);
		}

		bool should_explode() const { 
			// XXX this will keep the other from exploding when you kill one
			return !detonated_ && other_ != NULL;
		}
		float lifetime() const { return lifetime_; }
		int par_score() const { return should_explode() ? 5 : 0; }

	private:
		float wait_;
		vec pos_;
		PairBomb* other_;
		bool detonated_;
		float lifetime_;
	};

	void spawn_PairBomb_pair(vec apos, float difficulty = 1) {
		vec bpos;
		int safety = 0;
		do {
			if (safety++ > 50) break;
			bpos = generate_spawn_position();
		} while ((apos - bpos).norm() < 150);
		
		if ((apos - bpos).norm() < 150) return;
		
		PairBomb* a = new PairBomb(apos, 0.3, difficulty);
		PairBomb* b = new PairBomb(bpos, 0.3, difficulty);
		a->set_other(b);
		b->set_other(a);
		
		enemies_.push_back(a);
		enemies_.push_back(b);
		spawn_animation(apos);
		spawn_animation(bpos);
	}

	
	class VortexEnemy : public Enemy
	{
	public:
		VortexEnemy(vec pos, float wait, float difficulty) 
			: pos_(pos), wait_(wait), tex_(load_texture(ICHOR_DATADIR "/vortexball.png")),
			  lifetime_(0), total_density_(0), death_(false)
		{ }

		~VortexEnemy() {
			if (mode_) {
				mode_->field_->alter_balance(total_density_);
			}
		}

		void draw(const GalagaMode* mode) {
			if (wait_ > 0) return;
			glColor3f(1,1,1);
			TextureBinding b = tex_->bind();
			draw_rect(pos_ - vec(4,4), pos_ + vec(4,4));
		}

		void step(GalagaMode* mode) {
			wait_ -= DT;
			if (wait_ > 0) return;
			mode_ = mode;
			lifetime_ += DT;
			float radius = 10*(sin(lifetime_/3)+2);
			float directionp = -cos((lifetime_ - DT)/3) / fabs(cos((lifetime_ - DT)/3));
			float direction = -cos(lifetime_/3) / fabs(cos(lifetime_/3));
			if (directionp >= 0 && direction < 0) {
				death_ = true;
			}
			for (int i = 0; i < 96; i++) {
				float theta = 2*M_PI*i/96;
				vec dir = radius * vec(cos(theta), sin(theta));
				vec src = pos_ + dir;
				if  (src.x < CLAMPW || src.x >= W-CLAMPW
				  || src.y < CLAMPH || src.y >= H-CLAMPH) {
					continue;
				}
				if (direction > 0) {  // expanding
					total_density_ += mode->field_->get_density(src);
					mode->field_->set_density(src, 0);
				}
				else {                // contracting
					total_density_ += 0.5 * DT;
					mode->field_->add_density(src, -0.5);
				}
				mode->field_->add_velocity(src, 20*direction*~dir);
			}
			pos_ += mode->field_->get_velocity(pos_) * 70 * DT;
			pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
			pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
		}

		vec position() const { return pos_; }
		void set_position(vec pos) { pos_ = pos; }

		bool vulnerable() const { return wait_ <= 0; }
		float lifetime() const { return lifetime_; }
		int par_score() const { return 20; }
		bool dead(const GalagaMode* mode) const { return death_ || Enemy::dead(mode); } 

	private:
		vec pos_;
		float wait_;
		Texture* tex_;
		float lifetime_;
		float total_density_;
		GalagaMode* mode_;
		bool death_;
	};
	
	class SpinEnemy : public Enemy
	{
	public:
		SpinEnemy(vec pos, float wait, float difficulty) 
			: pos_(pos), wait_(wait), angle_(0), lifetime_(0), 
			real_lifetime_(0), tex_(load_texture(ICHOR_DATADIR "/spinball.png"))
		{ }

		void draw(const GalagaMode* mode) {
			if (wait_ > 0) return;
			glPushMatrix();
				glTranslatef(pos_.x, pos_.y, 0);
				glRotatef(180/M_PI*angle_, 0, 0, 1);
				glColor3f(1,1,1);
				TextureBinding b = tex_->bind();
				draw_rect(-vec(4,4), vec(4,4));
			glPopMatrix();
		}

		void step(GalagaMode* mode) {
			wait_ -= DT;
			if (wait_ > 0) return;
			real_lifetime_ += DT;
			
			if (pos_.x < CLAMPW) {
				angle_ = 0;//right
			}
			if (pos_.x > W - CLAMPW - 1) {
				angle_ = M_PI;//left
			}
			if (pos_.y < CLAMPH) {
				angle_ = M_PI/2;//up
			}
			if(pos_.y > H - CLAMPH - 1) {
				angle_ = 3*M_PI/2;//down
			}
			pos_.x = clamp(pos_.x, CLAMPW, W - CLAMPW - 1);
			pos_.y = clamp(pos_.y, CLAMPH, H - CLAMPH - 1);
			
			float largest = -1;
			vec largestPos;
			vec tempOffset = vec(15,0);
			float temp = mode->field_->get_density(pos_ + tempOffset);
			if (temp > 0) {
				if (temp > largest) largest = temp;
				largestPos += tempOffset * temp;
			}
			tempOffset = vec(-15, 0);
			temp = mode->field_->get_density(pos_ + tempOffset);
			if (temp > 0) {
				if (temp > largest) largest = temp;
				largestPos += tempOffset * temp;
			}
			tempOffset = vec(0, 15);
			temp = mode->field_->get_density(pos_ + tempOffset);
			if (temp > 0) {
				if (temp > largest) largest = temp;
				largestPos += tempOffset * temp;
			}
			tempOffset = vec(0, -15);
			temp = mode->field_->get_density(pos_ + tempOffset);
			if (temp > 0) {
				if (temp > largest) largest = temp;
				largestPos += tempOffset * temp;
			}

			mode->field_->add_density(pos_, -0.6);
			if (largest > -1e-4) {
				angle_ = atan2(largestPos.y, largestPos.x);
			}
			
			add_wide_velocity(mode->field_, pos_, 50*vec(cos(angle_),sin(angle_)));
			pos_ += mode->field_->get_velocity(pos_)* 10 * DT;
			
			if (lifetime_ > 5) lifetime_ = 5;
			else lifetime_ += DT/10;
			angle_ += DT / lifetime_;
		}

		vec position() const { return pos_; }
		void set_position(vec pos) { pos_ = pos; }
		bool vulnerable() const { return wait_ <= 0; }
		float lifetime() const { return real_lifetime_; }
	private:
		vec pos_;
		float wait_;
		float angle_;
		float lifetime_;
		float real_lifetime_;
		Texture* tex_;
	};
};

GameMode* make_GalagaMode(Input* inp) {
	return new GalagaMode(inp, time(NULL));
}
GameMode* make_GalagaMode(Input* inp, unsigned int seed) {
	return new GalagaMode(inp, seed);
}
GameMode* make_GalagaMode(ReaderInput* inp) {
	return new GalagaMode(inp);
}
