#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <gc.h>
#include <gc_allocator.h>
#include <gc_cpp.h>

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string>
using std::string;
#include <sstream>
using std::stringstream;

typedef double num;

extern const num C;    // units: m/s

extern const num ZL_GROUND;
extern const num ZG_GROUND;
extern const num ZL_TREES;
extern const num ZG_TREES;
extern const num ZL_SOLDIER;
extern const num ZG_SOLDIER;

extern const num FOREST_DENSITY;

extern const num SOLVER_TOLERANCE;

extern num CLOCK;

int _die(const string& msg, const char* file, int line);

#define DIE(msg) _die((string() + msg), __FILE__, __LINE__)

void _log(const string& msg);

#define LOG(msg) {\
    stringstream _logstream;\
    _logstream << msg;\
    _log(_logstream.str());\
  }

num randrange(num a, num b);

#endif
