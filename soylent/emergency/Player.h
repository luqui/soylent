#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <soy/ptr.h>
#include "Library.h"

const int STARTING_RESOURCES = 20;

template <class Rule>
class Player 
{
public:
	Player(typename Rule::Type color) : color_(color), libMan_(&library_), resources_(STARTING_RESOURCES) {
        if (color_ == Rule::PLAYER1) {
            place_ = Library<Rule>::BOTTOM;
        }
        else {
            place_ = Library<Rule>::TOP;
        }
		library_.set_place(place_);
    }

	void set_color(typename Rule::Type color) {//probably want to get rid of this later with refactor
		color_ = color;
	}

	typename Rule::Type get_color() {
		return color_;
	}

	void add_pattern(ptr< Pattern<Rule> > pattern) {
		libMan_.add_pattern(pattern, color_);
		library_.show_library();
	}

	void set_at(vec2 pos, typename Rule::State value, Universe<Rule>* u) {
		if (u->get_population().count[color_] < POP_CAP && resources_ > cost_at(pos)) {
            if (u->get_state_at(pos) != value) {
                resources_ -= cost_at(pos);
                u->set_at(pos, value);
            }
		}
	}

	void draw() const {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        vec2 mouse = VIEWSTACK.top().mouse_to_world(vec2(mx,my));

        if (selected_.valid()) {
            selected_->draw(int(mouse.x), int(mouse.y));
        }

		double xOffset = W/2 * resources_/999;
		if (color_ == Rule::PLAYER1) {
			glColor4f(1, 0, 0, ALPHA_FACTOR);

			glBegin(GL_QUADS);
				glVertex2f(W/2 - xOffset, 0);
				glVertex2f(W/2 - xOffset, H/80);
				glVertex2f(W/2 + xOffset, H/80);
				glVertex2f(W/2 + xOffset, 0);
			glEnd();
		}
		else {
			glColor4f(0, 1, 0, ALPHA_FACTOR);

			glBegin(GL_QUADS);
				glVertex2f(W/2 - xOffset, H - H/80);
				glVertex2f(W/2 - xOffset, H);
				glVertex2f(W/2 + xOffset, H);
				glVertex2f(W/2 + xOffset, H - H/80);
			glEnd();
		}
        
        libMan_.draw();
	}

	void rotate() { 
		if (selected_.valid()) {
			selected_ = selected_->rotate();
		}
	}

    bool l_click(vec2 mouse, Universe<Rule>* u) {
        string sel = libMan_.select(mouse);
		vec2 pos = VIEWSTACK.top().mouse_to_world(mouse);

        if (!sel.empty()) {
            selected_ = library_.get(sel);
			return true;
        }
        else if (selected_.valid()) {
			double cost_pos = 0;
			if (color_ == Rule::PLAYER1) {
				cost_pos = 1.0/(sqrt(2.0)) * selected_->get_size().y + pos.y;
			}
			else {
				cost_pos = pos.y - 1.0/(sqrt(2.0)) * selected_->get_size().y;
			}
			double cost = u->get_cost(selected_, pos) * cost_at(vec2(pos.x, cost_pos));
			if (cost <= resources_ && u->get_population().count[color_] < POP_CAP) {
				resources_ -= cost;
				u->insert_pattern(selected_, VIEWSTACK.top().mouse_to_world(mouse));
			}
			selected_ = 0;
			return true;
        }

		return false;
    }

	bool r_click(vec2 mouse, Universe<Rule>* u) {
		string sel;
		sel = libMan_.select(mouse);

		if (!sel.empty()) {
			library_.remove(sel);
			return true;
		}
		
		return false;
	}

	void famine(Universe<Rule>* u) {
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				if (u->get_state_at(vec2(i, j)).player == color_) {
					if (randrange(0,1) >= 0.5) u->set_at(vec2(i, j), Rule::dead_state());
				}
			}
		}
	}

    ptr< Pattern<Rule> > selected() const {
        return selected_;
    }

	void step() {
		if (resources_ < 999 && !PAUSE) {
			resources_ += DT*2;
		}

		library_.step();
	}

	void clear_resources() {
		resources_ = STARTING_RESOURCES;
	}

	void infinite_resources() {
		resources_ = 1e99;
	}

private:
	//This equation satisfies a cost of 1 at an offset of 0, 2 at an offset of 1/4th, and 10 at a full offset
	double cost_at(vec2 pos) const {
		if (color_ == Rule::PLAYER1) {
			return 20.0/(3.0*H*H) * pos.y*pos.y + 7.0/(3.0*H) * pos.y + 1.0;
		}
		else {
			return 20.0/(3.0*H*H) * (H-pos.y)*(H-pos.y) + 7.0/(3.0*H)*(H-pos.y) + 1.0;
		}
	}

	typename Rule::Type color_;
	LibraryManager<Rule> libMan_;
	double resources_;
	Library<Rule> library_;
    typename Library<Rule>::DrawPlace place_;
    ptr< Pattern<Rule> > selected_;
};

#endif
