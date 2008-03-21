#ifndef Port_h
#define Port_h

#ifdef WIN32

#include <windows.h>

#define PI 3.1415926535897932384626433832795

#pragma warning (disable: 4244 4305 4786)

typedef __int64 int64_t;

inline __int64 atoll(const char *aC) { return _atoi64(aC); }

#endif

#define int16 short
#define int32 int
#define uint32 unsigned int
#define bigtime_t __int64

#define MULTICHAR(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((a) << 24))

#endif 
