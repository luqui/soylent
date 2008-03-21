#ifndef __WIDGET_H__
#define __WIDGET_H__

#include "config.h"
#include "vec.h"
#include "Draw.h"

class Widget : public gc
{
public:
    virtual ~Widget() { }

    virtual num gheight(const tvec& ref) const = 0;
    virtual num lheight(const tvec& ref) const = 0;
    virtual vec pos(const tvec& ref) const = 0;
    virtual void draw(Artist* artist, const tvec& ref) const {
        artist->enqueue(ref, this);
    }
    virtual void render() const = 0;
    virtual void step() { }
};

#endif
