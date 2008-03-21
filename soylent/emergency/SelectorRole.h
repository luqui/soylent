#ifndef __SELECTORROLE_H__
#define __SELECTORROLE_H__

#include "Universe.h"
#include "Utils.h"

template<class Rule> 
class SelectorRole : virtual public Universe<Rule>
{
public:
	SelectorRole() : selecting_(false) {}
	
	virtual ~SelectorRole() {}

	virtual bool events(SDL_Event* e) {
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		vec2 mousePos = vec2(mouseX, mouseY);
		mousePos = VIEWSTACK.top().mouse_to_world(mousePos);


		if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_RIGHT) {
			selecting_ = true;
			startPos_ = mousePos;
			return true;
		}
		else if (e->type == SDL_MOUSEMOTION) {
			endPos_ = mousePos;
			return true;
		}
		else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_RIGHT) {
			if (!selecting_) return false; 
			selecting_ = false;
			endPos_ = mousePos;
			on_select(get_start_pos(), get_end_pos());
			return true;
		}
		return false;
	}


	virtual void draw() const {
		if (!selecting_) return;

		vec2 start = get_start_pos();
		vec2 end   = get_end_pos();

		glLineWidth(3.0);
		glColor4f(0.3, 0.3, 1.0, ALPHA_FACTOR);
		glBegin(GL_LINE_LOOP);
			glVertex2d(start.x, start.y);
			glVertex2d(start.x, end.y);
			glVertex2d(end.x,   end.y);
			glVertex2d(end.x,   start.y);
		glEnd();
	}

	virtual void on_select(vec2 start, vec2 end) = 0;

private:
	vec2 get_start_pos() const {
		double x = endPos_.x > startPos_.x ? floor(startPos_.x) : ceil(startPos_.x);
		double y = endPos_.y > startPos_.y ? floor(startPos_.y) : ceil(startPos_.y);
		return vec2(x,y);
	}
	
	vec2 get_end_pos() const {
		double x = endPos_.x > startPos_.x ? ceil(endPos_.x) : floor(endPos_.x);
		double y = endPos_.y > startPos_.y ? ceil(endPos_.y) : floor(endPos_.y);
		return vec2(x,y);
	}

	bool selecting_;
	vec2 startPos_;
	vec2 endPos_;
};


#endif
