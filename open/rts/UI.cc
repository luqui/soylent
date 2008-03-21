#include "UI.h"
#include "config.h"
#include <soy/vec2.h>
#include <soy/Viewport.h>
#include <soy/init.h>
#include "Board.h"
#include "Unit.h"
#include "util.h"
#include "ruby_interop.h"

class MainUI : public UI {
public:
	MainUI() : state_(NULL), camadjust_(false), dragging_(false), viewport_(vec2(16,12), vec2(32,24)) { }

	void step() {
		if (state_) state_->step();
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		camadjust_ = false;
		if (mx < 5) { viewport_.center.x -= 0.5*viewport_.dim.x*DT; camadjust_ = true; }
		if (mx > INIT.width() - 5) { viewport_.center.x += 0.5*viewport_.dim.x*DT; camadjust_ = true; }
		if (my < 5) { viewport_.center.y += 0.5*viewport_.dim.y*DT; camadjust_ = true; }
		if (my > INIT.height() - 5) { viewport_.center.y -= 0.5*viewport_.dim.y*DT; camadjust_ = true; }

		Uint8* keys = SDL_GetKeyState(NULL);
		if (keys[SDLK_z]) viewport_.dim /= 1+DT;
		if (keys[SDLK_x]) viewport_.dim *= 1+DT;
		if (viewport_.dim.x > BOARD.width())  viewport_.dim.x = BOARD.width();
		if (viewport_.dim.y > BOARD.height()) viewport_.dim.y = BOARD.height();
		
		if (viewport_.center.x - viewport_.dim.x/2 < 0)
			viewport_.center.x = viewport_.dim.x/2;
		if (viewport_.center.x + viewport_.dim.x/2 > BOARD.width())
			viewport_.center.x = BOARD.width() - viewport_.dim.x/2;
		if (viewport_.center.y - viewport_.dim.y/2 < 0)
			viewport_.center.y = viewport_.dim.y/2;
		if (viewport_.center.y + viewport_.dim.y/2 > BOARD.height())
			viewport_.center.y = BOARD.height() - viewport_.dim.y/2;
	}

	void draw() const {
		glPushMatrix();
		viewport_.activate();
		BOARD.draw(viewport_);
		for (std::list<Unit*>::iterator i = UNITS.begin(); i != UNITS.end(); ++i) {
			(*i)->draw();
		}
		//DUST.draw();

		if (dragging_) {
			vec2 mouse = mouse_pos(viewport_);
			if ((mouse - drag_source_).norm2() > 0.1) {
				glColor3f(1,1,1);
				glBegin(GL_LINE_LOOP);
				glVertex2f(drag_source_.x, drag_source_.y);
				glVertex2f(drag_source_.x, mouse.y);
				glVertex2f(mouse.x, mouse.y);
				glVertex2f(mouse.x, drag_source_.y);
				glEnd();
			}
		}
		glPopMatrix();
		
		if (state_) state_->draw();
	}

	bool events(SDL_Event* e) {
		if (state_ && state_->events(e)) return true;
		if (e->type          == SDL_MOUSEBUTTONDOWN &&
		    e->button.button == SDL_BUTTON_LEFT) {
				dragging_ = true;
				drag_source_ = mouse_pos(viewport_, e->button.x, e->button.y);
				return true;
		}
		if (e->type          == SDL_MOUSEBUTTONUP && 
		    e->button.button == SDL_BUTTON_LEFT && dragging_) {
				dragging_ = false;
				vec2 pos = mouse_pos(viewport_, e->button.x, e->button.y);
				if ((pos - drag_source_).norm2() > 0.1) {
					select_box(drag_source_, pos);
				}
				else {
					click_on(pos);
				}
				return true;
		}
		return false;
	}
	
private:
	void select_box(vec2 c1, vec2 c2) {
		delete state_;
		state_ = 0;

		std::list<Unit*> sels;
		BoundingBox c(c1, c2);
		for (std::list<Unit*>::iterator i = UNITS.begin(); i != UNITS.end(); ++i) {
			BoundingBox b = (*i)->get_bounding_box();
			if (intersects(c, b)) {
				sels.push_back(*i);
			}
		}
		if (sels.size() == 1) {
			state_ = new UnitSelectedState(this, sels.front());
		}
		else if (sels.size() > 1) {
			state_ = new MultiSelectedState(this, sels);
		}
	}

	void click_on(vec2 c) {
		delete state_;
		state_ = 0;
		for (std::list<Unit*>::iterator i = UNITS.begin(); i != UNITS.end(); ++i) {
			BoundingBox b = (*i)->get_bounding_box();
			if (intersects(c, b)) {
				state_ = new UnitSelectedState(this, *i);
				break;
			}
		}
	}

	UI* state_;
	bool camadjust_;
	bool dragging_;
	vec2 drag_source_;
	Viewport viewport_;

	class UnitSelectedState : public UI {
	public:
		UnitSelectedState(MainUI* ui, Unit* unit) 
			: stop_(true), camcenter_(false), ui_(ui), unit_(unit), motion_(NULL)
		{ }

		~UnitSelectedState() {
			if (stop_ && motion()) motion()->set_velocity(vec2());
		}

		void step() {
			bool moving = false;
			if (motion()) {
				Uint8* keys = SDL_GetKeyState(NULL);
				vec2 dir;
				if (keys[SDLK_w]) dir.y += 1;
				if (keys[SDLK_a]) dir.x -= 1;
				if (keys[SDLK_s]) dir.y -= 1;
				if (keys[SDLK_d]) dir.x += 1;
				if (dir.norm2() > 0) {
					stop_ = true;
					moving = true;
					motion()->set_ai(Qnil);
					motion()->set_velocity(motion()->get_speed_aptitude().get_max_speed() * ~dir);
				}
			}

			vec2 campos = motion()->get_position() + 0.8 * motion()->get_velocity();  // lead .8 seconds ahead
			num camdist = (campos - ui_->viewport_.center).norm() / ui_->viewport_.dim.norm();
			if (moving) {
				if (camdist > 0.3) {
					camcenter_ = true;
				}
			}

			if (camcenter_) {
				camvel_ += DT * ((campos - ui_->viewport_.center) - 2*(camvel_ - motion()->get_velocity()));
				ui_->viewport_.center += DT * camvel_;
				if (!moving && (ui_->camadjust_ || camvel_.norm() < 1)) {
					camcenter_ = false;
					camvel_ = 0;
				}
			}
		}

		bool events(SDL_Event* e) {
			if (motion()) {
				if (e->type == SDL_KEYUP && (
						e->key.keysym.sym == SDLK_w ||
						e->key.keysym.sym == SDLK_a ||
						e->key.keysym.sym == SDLK_s ||
						e->key.keysym.sym == SDLK_d)) {
					motion()->set_velocity(vec2());
					return true;
				}
				if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_RIGHT) {
					vec2 dest = mouse_pos(ui_->viewport_, e->button.x, e->button.y);
					VALUE ai = rb_funcall(rb_path2class("PathFinder"), rb_intern("new"), 1, rbw_ptr_to_value<Unit>(motion(), "MobileUnit"));
					stop_ = false;
					rbw_deliver_message(ai, rb_funcall(rb_path2class("AI::MoveTo"), rb_intern("new"), 1,
								rbw_vec2_to_value(dest)));
					return true;
				}
			}
			return false;
		}

		void draw() const {
			glColor3f(1,1,1);
			draw_selection_box(unit_->get_bounding_box());
		}
		
	private:
		MobileUnit* motion() {
			if (motion_) { 
				return motion_; 
			}
			else {  // XXX cache failure too
				return motion_ = dynamic_cast<MobileUnit*>(unit_); 
			}
		}
		
		bool stop_;
		bool camcenter_;
		vec2 camvel_;
		MainUI* ui_;
		Unit* unit_;
		MobileUnit* motion_;
	};

	class MultiSelectedState : public UI {
	public:
		MultiSelectedState(MainUI* ui, const std::list<Unit*> units) : ui_(ui), units_(units) { }

		bool events(SDL_Event* e) {
			if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_RIGHT) {
				VALUE movables = rb_ary_new();
				for (std::list<Unit*>::const_iterator i = units_.begin(); i != units_.end(); ++i) {
					if (MobileUnit* mob = dynamic_cast<MobileUnit*>(*i)) {
						rb_ary_push(movables, rbw_ptr_to_value<Unit>(mob, "MobileUnit"));
					}
				}

				if (RARRAY(movables)->len > 0) {
					vec2 dest = mouse_pos(ui_->viewport_, e->button.x, e->button.y);
					VALUE ai = rb_funcall(rb_path2class("FormationPathFinder"), rb_intern("new"), 1, movables);
					rbw_deliver_message(ai, rb_funcall(rb_path2class("AI::MoveTo"), rb_intern("new"), 1,
								rbw_vec2_to_value(dest)));
				}
					
				return true;
			}

			return false;
		}
		
		void draw() const {
			glColor3f(1,1,1);
			for (std::list<Unit*>::const_iterator i = units_.begin(); i != units_.end(); ++i) {
				draw_selection_box((*i)->get_bounding_box());
			}
		}
		
	private:
		MainUI* ui_;
		std::list<Unit*> units_;
	};
};

UI* make_MainUI() {
	return new MainUI;
}
