#include "Relativity.h"
#include <utility>
#include <math.h>

static std::pair<num,num> light_intersection(
        const tvec& r, const vec& v, tvec frame)
{
    frame -= r;
    num a = C*C - v.x*v.x - v.y*v.y;
    num b = -2*C*C*frame.c + 2*v.x*frame.p.x + 2*v.y*frame.p.y;
    num c = C*C*frame.c*frame.c - frame.p.x*frame.p.x - frame.p.y*frame.p.y;
    num disc = sqrt(b*b - 4*a*c);
    return std::pair<num,num>(r.c + (-b - disc) / (2*a), r.c + (-b + disc) / (2*a));
}

num light_line_to_point(const tvec& r, const vec& v, const tvec& frame)
{
    return light_intersection(r,v,frame).first;
}

num light_point_to_line(const tvec& r, const vec& v, const tvec& frame)
{
    return light_intersection(r,v,frame).second;
}
