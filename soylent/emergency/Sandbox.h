#ifndef __SANDBOX_H__
#define __SANDBOX_H__

#include "Universe.h"

class Sandbox : virtual public Universe<TwoPlayerStarWarsRule>
{
public:
    typedef TwoPlayerStarWarsRule Rule;

    Sandbox(int w, int h) 
        : Universe<Rule>(w,h, Rule()) 
    { }
};

#endif
