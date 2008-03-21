#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <list>

typedef double num;

const num DT = 1/30.0;

class Board;
extern Board BOARD;

class SoyInit;
extern SoyInit INIT;

class Dust;
extern Dust DUST;

class Scheduler;
extern Scheduler SCHEDULER;

class Unit;
extern std::list<Unit*> UNITS;

extern num GAMETIME;

#endif
