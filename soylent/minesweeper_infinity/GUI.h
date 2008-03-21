#ifndef __GUI_H__
#define __GUI_H__

#include "config.h"
#include <string>
#include <guichan.hpp>
#include <guichan/sdl.hpp>

void loadGUIPage(std::string pageName);
bool inTab(int x, int y);

#endif
