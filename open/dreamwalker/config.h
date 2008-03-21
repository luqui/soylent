#ifndef __CONFIG_H__
#define __CONFIG_H__

class Level;

extern Level* LEVEL;
extern float DT;

class vec2;
extern vec2 MOUSE;

class Dude;
extern Dude* DUDE;

class MouseInterpreter;
extern MouseInterpreter INTERP;

class SoyInit;
extern SoyInit INIT;

extern bool FALLTHROUGH;

void reset();

#endif
