#ifndef __CONTROLROLE_H__
#define __CONTROLROLE_H__

#include "Universe.h"
#include "TwoPlayerStarWarsRule.h"
#include "Player.h"

template<class Rule> 
inline void create_line(vec2 start, vec2 end, typename Rule::State state, Player<Rule>* player, Universe<Rule>* u) {
	float step = 1 / (end - start).norm();
	for (float t = 0; t <= 1; t += step) {
		vec2 p = start + (end - start) * t;
		if (u->in_bounds(p)) {
			player->set_at(p, state, u);
		}
	}
}

class ControlRole : virtual public Universe<TwoPlayerStarWarsRule>
{
public:
	typedef TwoPlayerStarWarsRule Rule;

	ControlRole() :player1_(Rule::PLAYER1), player2_(Rule::PLAYER2), drawPlayer_(&player1_), drawState_(Rule::State()), drawing_(false) { }

	virtual bool events(SDL_Event* eptr) {
		SDL_Event& e = *eptr;
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_c) {
			if (drawPlayer_ == &player1_) drawPlayer_ = &player2_;
			else						  drawPlayer_ = &player1_;
		}

		if (e.type == SDL_MOUSEBUTTONDOWN) {
            mouse_ = vec2(e.button.x, e.button.y);
            vec2 worldMouse = lastWorldMouse_ = VIEWSTACK.top().mouse_to_world(mouse_);

            if (in_bounds(worldMouse)) {
                if(e.button.button == SDL_BUTTON_LEFT) {
					drawing_ = true;
                    Rule::State s = get_state_at(worldMouse);
                    if (s.player == drawPlayer_->get_color()) {
						drawPlayer_->set_at(worldMouse, Rule::State(Rule::DEAD, 0), this);
                    }
                    else {
						drawPlayer_->set_at(worldMouse, get_rule().alive_state(drawPlayer_->get_color()), this);
					}
					drawState_ = get_state_at(worldMouse);
					return true;
				}
			}
		}

		if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
			drawing_ = false;
            mouse_ = vec2(e.button.x, e.button.y);
            lastWorldMouse_ = VIEWSTACK.top().mouse_to_world(mouse_);
		}

		if (e.type == SDL_MOUSEMOTION) {
			mouse_ = vec2(e.motion.x, e.motion.y);

			if(drawing_) {
				vec2 worldMouse = VIEWSTACK.top().mouse_to_world(mouse_);
				create_line(lastWorldMouse_, worldMouse, drawState_, drawPlayer_, this);
				lastWorldMouse_ = worldMouse;
			}
			return false;  // XXX mousemotion needs to be handled by multiple things.
			               // We want a better design.
		}
		return false;
	}

protected:
	Player<Rule> player1_;
	Player<Rule> player2_;
	Player<Rule>* drawPlayer_;//XXX try to make this private again with refactor

private:
	Rule::State drawState_;
	bool drawing_;
	vec2 lastWorldMouse_;
	vec2 mouse_;
};

#endif
