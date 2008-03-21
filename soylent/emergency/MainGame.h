#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Universe.h"
#include "SelectorRole.h"
#include "ControlRole.h"
#include "Player.h"
#include <soy/Font.h>
#include <soy/util.h>
#include <list>

class Target
{
public:
	typedef TwoPlayerStarWarsRule Rule;

	Target(Rule::Type color, vec2 ll, vec2 ur) : color_(color), ll_(ll), ur_(ur), alive_(true){ }

	void step(Universe<Rule>* uni) {
		inside(uni);
	}

	void draw() const {
		if (alive_) {
			if (color_ == Rule::PLAYER1) {
				glColor4f(1, 0, 0, ALPHA_FACTOR);
			}
			else {
				glColor4f(0, 1, 0, ALPHA_FACTOR);
			}
		}
		else {
			glColor4f(1, 1, 1, ALPHA_FACTOR);
		}
		glLineWidth(3.0);
		glBegin(GL_LINE_LOOP);
			glVertex2f(ll_.x, ll_.y);
			glVertex2f(ll_.x, ur_.y);
			glVertex2f(ur_.x, ur_.y);
			glVertex2f(ur_.x, ll_.y);
		glEnd();
	}

private:
	void inside(Universe<Rule>* uni) {
		if (!alive_) return;
		for (int i = int(ll_.x); i < int(ur_.x); i++) {
			for (int j = int(ll_.y); j < int(ur_.y); j++) {
				if (uni->get_state_at(vec2(i, j)).player == Rule::other_player(color_)) {
					alive_ = false;
					return;
				}
			}
		}
	}

	Rule::Type color_;
	vec2 ll_;
	vec2 ur_;
	bool alive_;
};

class MainGame : virtual public Universe<TwoPlayerStarWarsRule>
	           , virtual public SelectorRole<TwoPlayerStarWarsRule>
			   , virtual public ControlRole
{
public:
    typedef TwoPlayerStarWarsRule Rule;
    
    MainGame(int width, int height)
		: Universe<Rule>(width, height, TwoPlayerStarWarsRule())
    {
		reset();
	}
    
	void reset() {
		reset_targets();
		player1_.clear_resources();
		player2_.clear_resources();
	}

	void reset_targets() {
		targets_.clear();
		targets_.push_back(Target(player1_.get_color(), vec2(W*0.25 - 3, H*0.1), vec2(W*0.25 + 3, H*0.1 + 6)));
		targets_.push_back(Target(player1_.get_color(), vec2(W*0.5 - 3, H*0.1), vec2(W*0.5 + 3, H*0.1 + 6)));
		targets_.push_back(Target(player1_.get_color(), vec2(W*0.75 - 3, H*0.1), vec2(W*0.75 + 3, H*0.1 + 6)));

		targets_.push_back(Target(player2_.get_color(), vec2(W*0.25 - 3, H*0.9 - 6), vec2(W*0.25 + 3, H*0.9)));
		targets_.push_back(Target(player2_.get_color(), vec2(W*0.5 - 3, H*0.9 - 6), vec2(W*0.5 + 3, H*0.9)));
		targets_.push_back(Target(player2_.get_color(), vec2(W*0.75 -3 , H*0.9 - 6), vec2(W*0.75 + 3, H*0.9)));
	}

    void draw() const {
        Universe<Rule>::draw();

		draw_grid();

		for (targets_t::const_iterator i = targets_.begin(); i != targets_.end(); i++) {
			i->draw();
		}

		SelectorRole<Rule>::draw();
		player1_.draw();
		player2_.draw();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,1,0,1);

        GlutFont* font = new GlutFont;
        {
            glColor4f(1, 0.5, 0.5, ALPHA_FACTOR);

			if (drawPlayer_->get_color() == Rule::PLAYER1) {
				glLineWidth(1);
				glBegin(GL_LINE_LOOP);
					glVertex2f(0.00, 0.00);
					glVertex2f(0.00, 0.03);
					glVertex2f(0.05, 0.03);
					glVertex2f(0.05, 0.00);
				glEnd();
			}

            std::stringstream ss;
            ss << population.count[Rule::PLAYER1];
            font->draw_string(vec3(0.01,0.01,0), 1, ss.str());
        }

        {
            glColor4f(0.5, 1, 0.5, ALPHA_FACTOR);

			if (drawPlayer_->get_color() == Rule::PLAYER2) {
				glLineWidth(1);
				glBegin(GL_LINE_LOOP);
					glVertex2f(0.00, 0.05);
					glVertex2f(0.00, 0.08);
					glVertex2f(0.05, 0.08);
					glVertex2f(0.05, 0.05);
				glEnd();
			}

            std::stringstream ss;
            ss << population.count[Rule::PLAYER2];
            font->draw_string(vec3(0.01,0.06,0), 1, ss.str());
        }

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        delete font;
    }

    void scroll(vec2 direction) {
        ViewBox oldbox = VIEWSTACK.top();
        float left   = clamp(double(0), double(W - oldbox.width),  oldbox.left   + direction.x*DT);
        float bottom = clamp(double(0), double(H - oldbox.height), oldbox.bottom + direction.y*DT);
        change_view(ViewBox(oldbox.width, oldbox.height, left, bottom));
    }
    
    void step() {
        int x, y;
        SDL_GetMouseState(&x,&y);
        double scale = VIEWSTACK.top().width;
        if (x > XRES - 10) {
            scroll(scale * vec2(SCROLL_SPEED,0));
        }
        else if (x < 10) {
            scroll(scale * vec2(-SCROLL_SPEED,0));
        }

        if (y > YRES - 10) {
            scroll(scale * vec2(0, -SCROLL_SPEED));
        }
        else if (y < 10) {
            scroll(scale * vec2(0, SCROLL_SPEED));
        }

		player1_.step();
		player2_.step();

		for (targets_t::iterator i = targets_.begin(); i != targets_.end(); i++) {
			i->step(this);
		}
    }

	bool events(SDL_Event* e) {
        if (e->type == SDL_MOUSEBUTTONDOWN) {
			vec2 rawmouse = vec2(e->button.x, e->button.y);

			if (e->button.button == SDL_BUTTON_LEFT) {
				if(drawPlayer_->l_click(rawmouse, this)) {
					return true;
				}
			}
			else if (e->button.button == SDL_BUTTON_RIGHT) {
				if (drawPlayer_->r_click(rawmouse, this)) {
					return true;
				}
			}
		}
		if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_TAB) {
				drawPlayer_->rotate();
				return true;
			}
			if (e->key.keysym.sym == SDLK_F5) {
				drawPlayer_->famine(this);
				return true;
			}
			if (e->key.keysym.sym == SDLK_RETURN) {
				drawPlayer_->infinite_resources();
			}
		}
		
		return ControlRole::events(e) || SelectorRole<Rule>::events(e);
	}

	void on_select(vec2 start, vec2 end) {
		drawPlayer_->add_pattern(Universe<Rule>::make_pattern(start, end));
	}

private:
	typedef std::list<Target> targets_t;
	targets_t targets_;
};

#endif
