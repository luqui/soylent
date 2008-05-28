#ifndef __TWEAK_H__
#define __TWEAK_H__

#ifdef WIN32
#  include <windows.h>
#  include <glut.h>
#else 
#  include <GL/glut.h>
#endif

#include "config.h"
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

extern bool QUIT;

const int W = 300;
const int H = 225;
const int CLAMPW = 20;
const int CLAMPH = 20;

const float INVINCIBLE_TIME = 3;
const float PARTICLE_RATE = 50;
const float DEATH_DENSITY = 3e-3;
const float FLOWSPEED = 1600;
const float INCOME_RATE = 1;
const float EMPTY_RATE = 2;
const float VISCOSITY = 0;
const float DIFFUSION = 0;//5e-6;
const float EATDIST = 9;
const float EATENERGY = 6.0/PARTICLE_RATE;
const float DIE_ENERGY = 0;
const float TIMESCALE = .5;
const float DEATH_FLUID = 1;
const float PARTICLEDRIFT = 10000;
const float PLAYER_MOTION = 35;
const float DT = 1/30.0;
const float COLOR_CHANGE_RATE = 0.25;
const float MAX_COLOR_CHANGE_RATE = 3.0;
const float MIN_COLOR_NORM = 0.75;

#endif
