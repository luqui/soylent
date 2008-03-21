#ifndef __SOYLIB_CONFIG_H__
#define __SOYLIB_CONFIG_H__

#ifdef WIN32
#  include <windows.h>
#  define DLLEXPORT __declspec(dllexport)
#else
#  define DLLEXPORT
#endif

#include <cmath>
#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

#ifndef __GNUC__
#define __attribute__(x)
#endif

#endif
