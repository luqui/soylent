#ifndef __TWEAK_H__
#define __TWEAK_H__

#ifdef WIN32
#  include <windows.h>
#endif

#  include <GL/glut.h>

#include "config.h"
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define NUM_SONGS 10
const string SONGS[NUM_SONGS] = {
	ICHOR_MUSICDIR "/A 40 Riding on The Wind.ogg",
	ICHOR_MUSICDIR "/Sacred Ruins.ogg",
	ICHOR_MUSICDIR "/Silent Winter.ogg",
	ICHOR_MUSICDIR "/The Sea.ogg",
	ICHOR_MUSICDIR "/Verbations.ogg",
	ICHOR_MUSICDIR "/Great Mountain.ogg",
	ICHOR_MUSICDIR "/Coyote on the Plains.ogg",
	ICHOR_MUSICDIR "/The Rainforest and the Witch Village.ogg",
	ICHOR_MUSICDIR "/Finale-Allegro Ma Non Troppo.ogg",
	ICHOR_MUSICDIR "/Allegro Leggiero.ogg"
};

extern bool QUIT;

const int W = 320;
const int H = 240;
const int CLAMPW = 1;
const int CLAMPH = 1;

const float INVINCIBLE_TIME = 3;
const float PARTICLE_RATE = 50;
const float DEATH_DENSITY = 5e-2; //fatal fluid density
const float FLOWSPEED = 1200; //player's shoot speed
const float INCOME_RATE = 4; //amount of density player outputs each step
const float EMPTY_RATE = 2;
const float VISCOSITY = 0;
const float DIFFUSION = 0;//5e-6;
const float EATDIST = 9;
const float EATENERGY = 6.0/PARTICLE_RATE;
const float DIE_ENERGY = 0;
const float TIMESCALE = 1;
const float DEATH_FLUID = 3; //amount fluid released in the death explosion
const float PARTICLEDRIFT = 5000;
const float PLAYER_MOTION = 1.8; //changes how much the player is affected by the fluid's velocity
const float DT = 1/30.0;
const float COLOR_CHANGE_RATE = 0.25;
const float MAX_COLOR_CHANGE_RATE = 3.0;
const float MIN_COLOR_NORM = 0.75;
const float COLOR_SCALE = 3.0;

#endif
