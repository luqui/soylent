#include "soy/ruby.h"

namespace soyrb 
{

#define RBW_FUNC(x) reinterpret_cast<VALUE(*)(...)>(x)
#define SDLCLASS(x) rb_path2class("Soy::SDL::" #x)
#define MANYMOUSECLASS(x) rb_path2class("Soy::ManyMouse::" #x)

static VALUE rbw_ManyMouse_device_name(VALUE self, VALUE index) {
    int idx = rb_num2long(index);
    const char* devname = ManyMouse_DeviceName(idx);
    if (devname) {
        return rb_str_new2(devname);
    }
    else {
        return Qnil;
    }
}

void init() 
{
    VALUE cls = rb_path2class("Soy::ManyMouse");
    rb_define_singleton_method(cls, "device_name", RBW_FUNC(&rbw_ManyMouse), 1);
}
 

VALUE to_value(const SDL_Event& e) {
    switch (e.type) {
    case SDL_ACTIVEEVENT: {
        VALUE obj = rb_obj_alloc(SDLCLASS(ActiveEvent));
        rb_iv_set(obj, "@gain",  INT2FIX(event.active.gain));
        rb_iv_set(obj, "@state", INT2FIX(event.active.state));
        return obj;
    }
    
    case SDL_KEYUP:
    case SDL_KEYDOWN: {
        VALUE obj = rb_obj_alloc(e.type == SDL_KEYUP ? SDLCLASS(KeyUpEvent) : SDLCLASS(KeyDownEvent));
        VALUE keysym = rb_obj_alloc(SDLCLASS(Keysym));
        rb_iv_set(keysym, "@scancode", INT2FIX(event.key.keysym.scancode));
        rb_iv_set(keysym, "@sym",      INT2FIX(event.key.keysym.sym));
        rb_iv_set(keysym, "@mod",      INT2FIX(event.key.keysym.mod));
        rb_iv_set(keysym, "@unicode",  INT2FIX(event.key.keysym.unicode));
        
        rb_iv_set(obj, "@state",    INT2FIX(event.key.state));
        rb_iv_set(obj, "@keysym",   keysym);
        return obj;
    }

    case SDL_MOUSEMOTION: {
        VALUE obj = rb_obj_alloc(SDLCLASS(MouseMotionEvent));
        rb_iv_set(obj, "@state", INT2FIX(event.motion.state));
        rb_iv_set(obj, "@x",     INT2FIX(event.motion.x));
        rb_iv_set(obj, "@y",     INT2FIX(event.motion.y));
        rb_iv_set(obj, "@xrel",  INT2FIX(event.motion.xrel));
        rb_iv_set(obj, "@yrel",  INT2FIX(event.motion.yrel));
        return obj;
    }

    case SDL_MOUSEBUTTONUP: 
    case SDL_MOUSEBUTTONDOWN: {
        VALUE obj = rb_obj_alloc(e.type == SDL_MOUSEBUTTONUP ? SDLCLASS(MouseButtonUpEvent) : SDLCLASS(MouseButtonDownEvent));
        rb_iv_set(obj, "@button", INT2FIX(event.button.button));
        rb_iv_set(obj, "@state",  INT2FIX(event.button.state));
        rb_iv_set(obj, "@x",      INT2FIX(event.button.x));
        rb_iv_set(obj, "@y",      INT2FIX(event.button.y));
        return obj;
    }

    case SDL_JOYAXISMOTION: {
        VALUE obj = rb_obj_alloc(SDLCLASS(JoyAxisEvent));
        rb_iv_set(obj, "@which",  INT2FIX(event.jaxis.which));
        rb_iv_set(obj, "@axis",   INT2FIX(event.jaxis.axis));
        rb_iv_set(obj, "@value",  INT2FIX(event.jaxis.value));
        return obj;
    }

    case SDL_JOYBALLMOTION: {
        VALUE obj = rb_obj_alloc(SDLCLASS(JoyBallEvent));
        rb_iv_set(obj, "@which", INT2FIX(event.jball.which));
        rb_iv_set(obj, "@ball",  INT2FIX(event.jball.ball));
        rb_iv_set(obj, "@xrel",  INT2FIX(event.jball.xrel));
        rb_iv_set(obj, "@yrel",  INT2FIX(event.jball.yrel));
        return obj;
    }

    case SDL_JOYHATMOTION: {
        VALUE obj = rb_obj_alloc(SDLCLASS(JoyHatEvent));
        rb_iv_set(obj, "@which", INT2FIX(event.jhat.which));
        rb_iv_set(obj, "@hat",   INT2FIX(event.jhat.hat));
        rb_iv_set(obj, "@value", INT2FIX(event.jhat.value));
        return obj;
    }

    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN: {
        VALUE obj = rb_obj_alloc(e.type == SDL_JOYBUTTONUP ? SDLCLASS(JoyButtonUpEvent) : SDLCLASS(JoyButtonDownEvent));
        rb_iv_set(obj, "@which",  INT2FIX(event.jbutton.which));
        rb_iv_set(obj, "@button", INT2FIX(event.jbutton.button));
        rb_iv_set(obj, "@state",  INT2FIX(event.jbutton.state));
        return obj;
    }

    case SDL_VIDEORESIZE: {
        VALUE obj = rb_obj_alloc(SDLCLASS(ResizeEvent));
        rb_iv_set(obj, "@w", INT2FIX(event.resize.w));
        rb_iv_set(obj, "@h", INT2FIX(event.resize.h));
        return obj;
    }

    case SDL_VIDEOEXPOSE: return rb_obj_alloc(SDLCLASS(ExposeEvent));
    case SDL_QUIT:        return rb_obj_alloc(SDLCLASS(QuitEvent));
    default: return Qnil;
    }
}

VALUE to_val(const ManyMouseEvent& e) {
    VALUE obj;
    switch (e.type) {
        case MANYMOUSE_EVENT_ABSMOTION: obj = rb_obj_alloc(MANYMOUSECLASS(AbsMotionEvent));
                                        break;
        case MANYMOUSE_EVENT_RELMOTION: obj = rb_obj_alloc(MANYMOUSECLASS(RelMotionEvent));
                                        break;
        case MANYMOUSE_EVENT_BUTTON:    obj = rb_obj_alloc(MANYMOUSECLASS(ButtonEvent));
                                        break;
        case MANYMOUSE_EVENT_SCROLL:    obj = rb_obj_alloc(MANYMOUSECLASS(ScrollEvent));
                                        break;
        case MANYMOUSE_EVENT_DISCONNECT:obj = rb_obj_alloc(MANYMOUSECLASS(DisconnectEvent));
                                        break;
        default:
            DIE("Uh, that's not any manymouse event I've ever heard of");
    }
    rb_iv_set(obj, "@device", INT2FIX(event.device));
    rb_iv_set(obj, "@item",   INT2FIX(event.item));
    rb_iv_set(obj, "@value",  INT2FIX(event.value));
    rb_iv_set(obj, "@minval", INT2FIX(event.minval));
    rb_iv_set(obj, "@maxval", INT2FIX(event.maxval));
    return obj;
}

}
