#ifndef __INPUT_H__
#define __INPUT_H__

#include "common.h"
#include "vec.h"

class MouseResponder {
public:
    virtual ~MouseResponder() { }

    virtual void mouse_motion_native(SDL_MouseMotionEvent* e) = 0;
    virtual void mouse_button_native(SDL_MouseButtonEvent* e) = 0;
};

extern MouseResponder* MOUSE_FOCUS;

class MouseSelector : public MouseResponder {
public:
    void mouse_motion_native(SDL_MouseMotionEvent* e);

    void mouse_button_native(SDL_MouseButtonEvent* e);

    virtual void mouse_move(vec delta)      { }
    virtual void mouse_wheel(int delta)     { }
    virtual void mouse_left_button_down()   { }
    virtual void mouse_left_button_up()     { }
    virtual void mouse_middle_button_down() { }
    virtual void mouse_middle_button_up()   { }
    virtual void mouse_right_button_down()  { }
    virtual void mouse_right_button_up()    { }
};

void events();

#endif
