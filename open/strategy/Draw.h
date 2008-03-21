#ifndef __DRAW_H__
#define __DRAW_H__

#include "config.h"
#include "vec.h"
#include <set>

using std::set;

class Widget;

class Artist : public gc {
public:
    void enqueue(const tvec& ref, const Widget* wid);
    void render() const;
    void clear();
private:
    struct DrawElem : public gc {
        DrawElem(const Widget* widget, num gheight, num lheight, vec pos) 
            : widget(widget), gheight(gheight), lheight(lheight), pos(pos)
        { }

        const Widget* widget;
        num gheight, lheight;
        vec pos;
    };

    struct DrawCompare {
        bool operator() (DrawElem* a, DrawElem* b) {
            if (a->gheight != b->gheight) return a->gheight < b->gheight;
            if (a->pos.y  != b->pos.y)    return a->pos.y  > b->pos.y;
            if (a->pos.x  != b->pos.x)    return a->pos.x  > b->pos.x;
            if (a->lheight != b->lheight) return a->lheight < b->lheight;
            return false;
        }
    };
    
    typedef set< DrawElem*, DrawCompare, gc_allocator< DrawElem* > > queue_t;
    queue_t queue_;
};

namespace Draw
{
    void texture_square();
}

#endif
