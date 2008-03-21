#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "common.h"
#include "drawing.h"
#include "input.h"
#include <list>

class Effect {
public:
    virtual ~Effect() { }
    virtual bool step() = 0;  // returns whether it is finished
    virtual void draw() = 0;
};

class EffectsManager : public MouseResponder {
public:
    void add(Effect* effect) { effects_.push_back(effect); }
    
    void step() {
        for (effects_t::iterator i = effects_.begin(); i != effects_.end();) {
            if ((*i)->step()) {
                effects_t::iterator j = i;  ++j;
                delete *i;
                effects_.erase(i);
                i = j;
            }
            else {
                ++i;
            }
        }
    }
    
    void draw() {
        for (effects_t::iterator i = effects_.begin(); i != effects_.end(); ++i) {
            (*i)->draw();
        }
    }

    bool done() const {
        return effects_.size() == 0;
    }

    void mouse_motion_native(SDL_MouseMotionEvent*) { }
    
    void mouse_button_native(SDL_MouseButtonEvent*) {
        for (effects_t::iterator i = effects_.begin(); i != effects_.end(); ++i) {
            delete *i;
        }
        effects_.clear();
    }
    
private:
    typedef list<Effect*> effects_t;
    effects_t effects_;
};

class FadeImageEffect : public Effect {
public:
    FadeImageEffect(const string& image, 
                    vec ll, vec ur,
                    num fade_in, num keep, num fade_out) 
        : image_(image), img_(new Texture(image)),
          ll_(ll), ur_(ur),
          fade_in_(fade_in), keep_(keep), fade_out_(fade_out),
          state_(FADING_IN), state_timer_(fade_in)
    { }

    ~FadeImageEffect() {
        delete img_;
    }

    bool step() {
        state_timer_ -= STEP;
        if (state_timer_ <= 0) {
            if (state_ == FADING_IN) {
                state_ = KEEPING; state_timer_ = keep_;
            }
            else if (state_ == KEEPING) {
                state_ = FADING_OUT; state_timer_ = fade_out_;
            }
            else if (state_ == FADING_OUT) {
                return true;
            }
        }
        return false;
    }

    void draw() {
        glPushAttrib(GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (state_ == FADING_IN) {
            glColor4d(1,1,1, 1 - state_timer_ / fade_in_);
        }
        else if (state_ == KEEPING) {
            glColor4d(1,1,1,1);
        }
        else if (state_ == FADING_OUT) {
            glColor4d(1,1,1, state_timer_ / fade_out_);
        }

        glBindTexture(GL_TEXTURE_2D, img_->tex_id());
        glBegin(GL_QUADS);
            glTexCoord2d(0,1);
            glVertex2d(ll_.x, ll_.y);
            glTexCoord2d(1,1);
            glVertex2d(ur_.x, ll_.y);
            glTexCoord2d(1,0);
            glVertex2d(ur_.x, ur_.y);
            glTexCoord2d(0,0);
            glVertex2d(ll_.x, ur_.y);
        glEnd();
        
        glPopAttrib();
    }
    
private:
    string image_;
    Texture* img_;

    vec ll_, ur_;

    num fade_in_, keep_, fade_out_;
    enum State { 
        FADING_IN, KEEPING, FADING_OUT
    };
    State state_;
    num state_timer_;
};

class DelayEffect : public Effect {
public:
    DelayEffect(num delay) : delay_(delay) { }

    bool step() {
        return (delay_ -= STEP) <= 0;
    }
    void draw() { }
    
private:
    num delay_;
};

class SeqEffect : public Effect {
public:
    SeqEffect(Effect* a, Effect* b)
        : fxa_(a), fxb_(b), state_(0)
    { }

    ~SeqEffect() {
        if (!state_) delete fxa_;
        delete fxb_;
    }

    bool step() {
        if (!state_) {
            if (fxa_->step()) {
                state_ = 1;
                delete fxa_;
            }
            return false;
        }
        else {
            return fxb_->step();
        }
    }

    void draw() {
        if (!state_) fxa_->draw();
        else        fxb_->draw();
    }

private:
    Effect* fxa_;
    Effect* fxb_;
    int state_;
};

#endif
