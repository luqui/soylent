#ifndef __RELATIVITY_H__
#define __RELATIVITY_H__

#include "config.h"
#include "vec.h"

// returns the minimum time at which light coming from anywhere on the line
// (a,avel) reached the fixed point b.
num light_line_to_point(const tvec& a, const vec& avel, const tvec& b);

// returns the minimum time at which light coming from the fixed point b
// reaches anywhere on the line (a,avel)
num light_point_to_line(const tvec& a, const vec& avel, const tvec& b);

#endif
