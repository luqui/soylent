#ifndef __CELL_H__
#define __CELL_H__

#include "BInterp.h"
#include <soy/vec2.h>
#include <iostream>

class Cell {
public:
    Cell(vec2 pos, BInterp* brain = 0, double foodtime = INITIAL_FOODTIME) 
        : pos_(pos), theta_(0), radius_(1), brain_(brain), foodtime_(foodtime) 
    { 
        if (!brain_) {
            brain_ = new BInterp(this);
        }
        else {
            brain_->set_parent(this);
        }
    }

    ~Cell() {
        delete brain_;
    }

    void randomize() {
        brain_->randomize();
        theta_ = randrange(0,2*M_PI);
        recompute_stats();
    }

    void mutate() {
        brain_->mutate();
        recompute_stats();
    }

    void eject(energy_t particle) {
        eject_direction(particle, random_unit_vector());
    }
    void eject_direction(energy_t particle, vec2 direction);

    void absorb(energy_t particle) {
        brain_->absorb(particle);
    }

    void draw() const {
        int sides = colors_.size();
        double sides_1 = 1.0 / sides;

        glColor3f(0.5,0.5,0.5);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(pos_.x,pos_.y);
        for (int i = 0; i < sides; i++) {
            const Color& c = colors_[i];
            glColor3f(c.r,c.g,c.b);
            double theta = 2 * M_PI * i * sides_1;
            glVertex2f(pos_.x + radius_ * cos(theta), pos_.y + radius_ * sin(theta));
        }
        {  // once for the last vertex, with the color of index 0
            const Color& c = colors_[0];
            glColor3f(c.r,c.g,c.b);
            glVertex2f(pos_.x + radius_, pos_.y);
        }
        glEnd();
    }

    void step() {
        brain_->step();
        pos_ += DT * vel_;
        vel_ *= 1 - CELL_DAMPING;

        foodtime_ -= DT;
    }

    bool alive() const {
        return foodtime_ > 0;
    }

    void die() {
        brain_->die();
    }

    void wrap_position(const Viewport& v) {
        pos_ = wrap_vector(v, pos_);
    }

    vec2 position() const { return pos_; }
    double radius() const { return radius_; }
    double rotation() const { return theta_; }

    void add_impulse(vec2 dir) {
        vel_ += dir.rotate(theta_);
    }
    void rotate(double by) {
        theta_ += by;
        while (theta_ > 2*M_PI) { theta_ -= 2*M_PI; }
        while (theta_ < 0)      { theta_ += 2*M_PI; }
    }

    Cell* reproduce(vec2 newpos, double newtheta, double foodtime, node_t top, int top_input) const {
        Cell* c = new Cell(pos_ + newpos.rotate(theta_), brain_->clone(top, top_input), foodtime);
        c->theta_ = newtheta;
        c->recompute_stats();
        return c;
    }

private:
    void recompute_stats() {
        radius_ = brain_->size();
        colors_ = brain_->colors();

        if (colors_.size() == 0) {
            for (int i = 0; i < 4; i++) {
                colors_.push_back(Color(1,1,1));
            }
        }

        while (colors_.size() < 4) {
            std::vector<Color> newcolors;
            for (std::vector<Color>::iterator i = colors_.begin(); i != colors_.end(); i++) {
                newcolors.push_back(*i);
                newcolors.push_back(*i);
            }
            colors_ = newcolors;
        }
    }

    vec2 pos_;
    double theta_;
    vec2 vel_;
    double radius_;
    BInterp* brain_;
    double foodtime_;
    std::vector<Color> colors_;
};

#endif
