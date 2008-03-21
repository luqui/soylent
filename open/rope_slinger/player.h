#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "common.h"
#include "vec.h"
#include "drawing.h"
#include "physics.h"
#include "geometry.h"
#include "input.h"
#include "objects.h"
#include <list>

class Player : public MouseSelector, public Object {
public:
    Player(vec pos, Color col) 
            : spikey_(0), angle_(0), geom_(pos, 1), selected_(0),
              color_(col) {
        body_.set_position(pos);
        body_.set_owner(this);
        geom_.set_owner(this);
        geom_.attach(&body_);
    }

    void step() {
        find_selected_rope();
    }

    void find_selected_rope() {
        if (this != LEVEL->player) {
            if (selected_) selected_->deselect();
            selected_ = 0;
            return;
        }
        Rope* best = 0;  num bestdiff = 1e999;
        for (ropes_t::iterator i = ropes_.begin(); i != ropes_.end(); ++i) {
            num diff = moddiff(angle_, (*i)->angle(), 2*M_PI);
            if (diff < bestdiff) {
                bestdiff = diff;
                best = *i;
            }
        }
        if (best == selected_) return;
        if (selected_) selected_->deselect();
        if (best) best->select();
        selected_ = best;
    }

    void draw() {
        vec pos = body_.position();
        find_selected_rope();
        glPushMatrix();
            glTranslated(pos.x, pos.y, 0);
            
            glColor3d(color_.r, color_.g, color_.b);
            draw_circle(1); 
            
            if (LEVEL->player == this) {
                glRotated(angle_ * 180 / M_PI, 0, 0, 1);
                glColor3d(1,1,1);
                glTranslated(2, 0, 0);
                draw_circle(0.1);
                glTranslated(2, 0, 0);
                draw_circle(0.2);
            }
        glPopMatrix();
    }

    void mouse_move(vec dir) {
        num amt = dir * vec(1,0) * -0.4;   // only horizontal motion
        angle_ += amt;
        angle_ = smallmod(angle_, 2*M_PI);
        find_selected_rope();
    }

    void mouse_left_button_down() {
        num dist = 1 + Spikey::radius;
        vec dir = vec(cos(angle_), sin(angle_));
        vec pos = body_.position();
        vec force = LEVEL->fire_velocity * dir / STEP;
        spikey_ = new Spikey(pos + dist*dir, force);
        body_.apply_force(-force, pos);  // newton's 3rd
        LEVEL->manager->add(spikey_);
        
        while (ropes_.size() >= LEVEL->max_ropes) {
            Rope* fst = ropes_.front();
            ropes_.pop_front();
            LEVEL->manager->remove(fst);
        }
        
        LEVEL->unfreeze();
    }

    void mouse_left_button_up() {
        if (!spikey_) return;
        Rope* rope = new Rope(this, &body_, spikey_, spikey_->body());
        ropes_.push_back(rope);
        LEVEL->manager->add(rope);

        spikey_ = 0;
        find_selected_rope();
    }

    void mouse_middle_button_down() {
        LEVEL->unfreeze();
    }

    void mouse_right_button_down() {
        if (spikey_) {
            spikey_ = 0; // right click not to attach rope
            return;
        }
        
        if (selected_) {
            for (ropes_t::iterator i = ropes_.begin(); i != ropes_.end(); ++i) {
                if (*i == selected_) {
                    ropes_.erase(i);
                    break;
                }
            }
            LEVEL->manager->remove(selected_);
            selected_ = 0;
            find_selected_rope();
        }
        LEVEL->unfreeze();
    }

    void mouse_wheel(int delta) {
        if (selected_) {
            selected_->lengthen(3*delta);
        }
    }

    void mark() {
        if (spikey_) LEVEL->manager->mark(spikey_);
        for (ropes_t::iterator i = ropes_.begin(); i != ropes_.end(); ++i) {
            LEVEL->manager->mark(*i);
        }
    }

    bool visible() const { return true; }
    
    bool dead() const {
        vec pos = body_.position();
        if (pos.x + 1 < LEVEL->left   || pos.x - 1 > LEVEL->right
         || pos.y + 1 < LEVEL->bottom || pos.y - 1 > LEVEL->top)
                return true;
        else return false;
    }

    Color color() const { return color_; }

private:
    Player(const Player&);  // no copying of players

    Spikey* spikey_;

    num angle_;
    Body body_;
    Circle geom_;

    Rope* selected_;
    typedef list<Rope*> ropes_t;
    ropes_t ropes_;

    Color color_;
};

#endif
