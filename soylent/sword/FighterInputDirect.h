#ifndef FighterInputDirect_h
#define FighterInputDirect_h

#include "Input.h"
#include "Fighter.h"

class FighterInput : public Input
{
public:
    FighterInput(Fighter* fighter) 
        : fighter_(fighter)
    { }
    
    void move(int x, int y) {
        fighter_->move_sword(vec2(0.01 * x, -0.01 * y));
    }

    void enter_temp_stance(ptr<Stance> stance) {
        restore_stance_.push(fighter_->change_stance(stance));
    }

    void restore_stance() {
        if (!restore_stance_.empty()) {
            fighter_->change_stance(restore_stance_.top());
            restore_stance_.pop();
        }
    }
    
    // subclasses override the event handlers, calling move() from within.
    // however, they should chain call their parents.


protected:
    // this is an abstract class, even though it may not look like it
    ~FighterInput() { }
    
    Fighter* fighter_;
    std::stack< ptr<Stance> > restore_stance_;
};

class FighterMouseInput : public FighterInput
{
public:
    FighterMouseInput(Fighter* fighter, const KeyMapping& keys)
        : FighterInput(fighter)
        , keys_(keys)
    { }

    void handle_SDL_Event(SDL_Event* e) {
        FighterInput::handle_SDL_Event(e);

        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == keys_.key("horizontal-block")) {
            enter_temp_stance(new HorizontalBlockStance);
        }
        
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == keys_.key("vertical-block")) {
            enter_temp_stance(new VerticalBlockStance);
        }

        if (e->type == SDL_KEYUP && 
                (e->key.keysym.sym == keys_.key("horizontal-block")
              || e->key.keysym.sym == keys_.key("vertical-block"))) {
            restore_stance();
        }
    }
protected:
    ~FighterMouseInput() { }
    
    KeyMapping keys_;
};

class FighterSDLMouseInput : public FighterMouseInput
{
public:
    FighterSDLMouseInput(Fighter* fighter, const KeyMapping& keys)
        : FighterMouseInput(fighter, keys)
    { }

    // TODO trickery:
    // On Luke's computer, the touchpad does not register as a 
    // manymouse mouse, but we need to use it as a mouse.
    // But if there is another mouse present, the touchpad and
    // the mouse will superpose, so we need to subtract off the
    // other mouse to use SDL's accurately.
    
    void handle_SDL_Event(SDL_Event* e) {
        FighterMouseInput::handle_SDL_Event(e);

        if (e->type == SDL_MOUSEMOTION) {
            move(e->motion.xrel, e->motion.yrel);
        }
    }
};

class FighterManyMouseInput : public FighterMouseInput
{
public:
    FighterManyMouseInput(Fighter* fighter, const KeyMapping& keys, unsigned int mouseid)
        : FighterMouseInput(fighter, keys)
        , mouseid_(mouseid)
    { }

    void handle_ManyMouseEvent(ManyMouseEvent* e) {
        FighterMouseInput::handle_ManyMouseEvent(e);

        if (e->device == mouseid_ && e->type == MANYMOUSE_EVENT_RELMOTION) {
            if (e->item == 0) {
                move(e->value, 0);
            }
            else if (e->item == 1) {
                move(0, e->value);
            }
        }

        if (e->device == mouseid_ && e->type == MANYMOUSE_EVENT_ABSMOTION) {
            DIE("Received an ABSMOTION event; I never thought that would happen!");
        }
    }

private:
    unsigned int mouseid_;
};

class FighterJoyInput : public FighterInput
{
public:
    FighterJoyInput(Fighter* fighter, int joyid)
        : FighterInput(fighter)
        , joyid_(joyid), stance_(NONE)
    { }

    void handle_SDL_Event(SDL_Event* e) {
        FighterInput::handle_SDL_Event(e);

		if (e->type == SDL_JOYAXISMOTION && e->jaxis.which == joyid_) {
			if (e->jaxis.axis == 0) { // x
				sword_pos_.x = float(e->jaxis.value) / 32768;
			}
			if (e->jaxis.axis == 1) { // y
				sword_pos_.y = -float(e->jaxis.value) / 32768;
			}
	
			if (e->jaxis.axis == 2) { // z
				if (e->jaxis.value > 1000 && stance_ != HORIZONTAL) {
					enter_temp_stance(new HorizontalBlockStance);
					stance_ = HORIZONTAL;
				}
				else if (e->jaxis.value < -1000 && stance_ != VERTICAL) {
					enter_temp_stance(new VerticalBlockStance);
					stance_ = VERTICAL;
				}
				else if (e->jaxis.value > -1000 && e->jaxis.value < 1000) {
					restore_stance();
					stance_ = NONE;
				}
			}
			fighter_->set_sword_pos(sword_pos_);
		}
    }
private:
    vec2 sword_pos_;
    int joyid_;
	enum {HORIZONTAL, VERTICAL, NONE} stance_;
};

#endif
