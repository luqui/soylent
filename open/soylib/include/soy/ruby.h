#ifndef __SOY_RUBY_H__
#define __SOY_RUBY_H__

#include "soy/config.h"
#include <SDL.h>
#include <manymouse.h>
#include <ruby.h>

namespace soyrb {

void DLLEXPORT init();

VALUE DLLEXPORT to_val(const SDL_Event& e);
VALUE DLLEXPORT to_val(const ManyMouseEvent& e);

};

#endif
