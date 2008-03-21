#ifndef DuelModeInputDirect_h
#define DuelModeInputDirect_h

#include "DuelMode.h"
#include "FighterInputDirect.h"

extern InputManager INPUT;

class DuelModeInputDirect : public DuelMode
{
public:
	DuelModeInputDirect()
	{
        // XXX need to clean up after myself
        // Maybe InputManager needs a richer interface that will
        // do that for me.
        INPUT.add_input(new InputAssigner(this));
	}

private:
    // This class's job is to dynamically assign input methods based
    // on what it detects is available.  The first mouse to move gets
    // first player, the second to move gets second player.
    class InputAssigner : public Input
    {
    public:
        InputAssigner(DuelMode* mode) : mode_(mode), assigned_(0) { }
        
        // XXX What about on most computers (but not Luke's) where the main mouse 
        // is also a manymouse mouse?
        void handle_SDL_Event(SDL_Event* e) {
            if (assigned_ >= 2) { return; }
            
            if (e->type == SDL_MOUSEMOTION && used_.type != Used::SYSMOUSE) {
                // XXX need to clean up after myself
                INPUT.add_input(new FighterSDLMouseInput(fighter(), mapping()));
                assigned_++;
                used_.type = Used::SYSMOUSE;
            }

            if (e->type == SDL_JOYAXISMOTION && 
                    (used_.type != Used::JOYSTICK || used_.data != e->jaxis.which)) {
                INPUT.add_input(new FighterJoyInput(fighter(), e->jaxis.which));
                assigned_++;
                used_.type = Used::JOYSTICK;
				used_.data = e->jaxis.which;
            }
        }

        void handle_ManyMouseEvent(ManyMouseEvent* e) {
            if (assigned_ >= 2) { return; }
            if (assigned_ == 1 && used_.type == Used::MOUSE
                    && used_.data == int(e->device)) {
                return;
            }
            
            if (e->type == MANYMOUSE_EVENT_RELMOTION) {
                INPUT.add_input(new FighterManyMouseInput(fighter(), mapping(), e->device));
                assigned_++;
                used_.type = Used::MOUSE;
                used_.data = e->device;
            }
        }

    private:
        Fighter* fighter() const {
            if (assigned_ == 0) return &mode_->player1_;
            if (assigned_ == 1) return &mode_->player2_;
            DIE("Tried to assign a fighter to player 3 (who doesn't exist)");
			return 0;	// stifle compile warning
        }

        KeyMapping mapping() const {
            KeyMapping keys;
            if (assigned_ == 0) {
                keys.bind(SDLK_a, "horizontal-block");
                keys.bind(SDLK_q, "vertical-block");
            }
            else if (assigned_ == 1) {
                keys.bind(SDLK_KP4, "horizontal-block");
                keys.bind(SDLK_KP7, "vertical-block");
            }
            else {
                DIE("Tried to assign a keymap to fictional player 3");
            }
            return keys;
        }
            
        DuelMode* mode_;
        
        // used_ keeps track of which input device has already been assigned,
        // when assigned_ == 1 (i.e. when one has been assigned).
        struct Used {
            enum { JOYSTICK, MOUSE, SYSMOUSE } type;
            int data;
        };
        Used used_;

        int assigned_;
    };
};

#endif
