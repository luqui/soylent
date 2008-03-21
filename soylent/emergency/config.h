#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef WIN32
#include <windows.h>
#endif

#include "ViewBox.h"

extern float ASPECT_RATIO;

extern int XRES;
extern int YRES;
extern int W;
extern int H;

extern double ALPHA_FACTOR;

extern bool PAUSE;

extern int zoomWidth;
extern int zoomHeight;

extern float DT;

#ifdef NDEBUG
const bool FULLSCREEN = true;
#else
const bool FULLSCREEN = false;
#endif

const int POP_CAP = 1000;

const float FRAMETIME = 0.05;
const float FILL_PERCENT = 0.05;

const int CONWAY_S = 0xc; //0b1100
const int CONWAY_B = 0x8; //0b1000
const int STAR_WARS_S = 0x38; //0b111000
const int STAR_WARS_B = 0x4; //0b100
const int STAR_WARS_C = 4;
const int TEST_S = 0xc;
const int TEST_B = 0x8;
const int TEST_C = 50;

const float SCROLL_SPEED = 0.5;  // in screen-widths-per-second

#endif
