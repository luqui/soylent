#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include "common.h"
#include "drawing.h"
#include "geometry.h"
#include "level.h"
#include <set>

class Object {
    friend class ObjectManager;
public:
    virtual ~Object() { }

    virtual bool visible() const = 0;
    virtual void mark() { }
    virtual void step() { }
    virtual void draw() { }

protected:
    Object() : visible_(false) { }

private:
    Object(const Object&);  // no copying
    
    bool visible_;
    Object* gc_next_;
};

class ObjectManager { 
public:
    ~ObjectManager() {
        for (set_t::iterator i = oset_.begin(); i != oset_.end(); ++i) {
            Object* o = *i;
            delete o;
        }
    }
    
    void add(Object* o) {
        oset_.insert(o);
    }

    void remove(Object* o) {
        oset_.erase(o);
        delete o;
    }

    void mark(Object* o);

    void sweep();

    void step() {
        for (set_t::iterator i = oset_.begin(); i != oset_.end(); ++i) {
            (*i)->step();
        }
    }
    
    void draw() {
        for (set_t::iterator i = oset_.begin(); i != oset_.end(); ++i) {
            (*i)->draw();
        }
    }

private:
    Object* gc_tail_;
    
    typedef set<Object*> set_t;
    set_t oset_;
};

class Wall : public Object {
public:
    Wall(vec ll, vec ur) : ll_(ll), ur_(ur), geom_(ll, ur) {
        geom_.set_owner(this);
    }

    bool visible() const { return true; }
    
    void draw() {
        glColor3d(0.7, 0.7, 0.7);
        draw_box(ll_, ur_);
    }
private:
    vec ll_, ur_;
    Box geom_;
};

class Spikey : public Object {
public:
    enum State {
        STICKY,
        STUCK,
        UNSTUCK
    };
    
    Spikey(vec p, vec f);
    ~Spikey() { if (hinge_) dJointDestroy(hinge_); }

    bool visible() const {
        vec p = body_.position();
        // we don't check off the top of the screen, because stuff falls.
        if (p.x < LEVEL->left || p.x > LEVEL->right ||
            p.y < LEVEL->bottom) return false;
        else return true;
    }

    void draw() {
        vec p = body_.position();
        glPushMatrix();
            glTranslated(p.x, p.y, 0);
            if (state_ == UNSTUCK) glColor3d(0.75,0.5,0);
            else                   glColor3d(0, 0.75, 0);
            draw_circle(radius);
        glPopMatrix();
    }

    Body* body() { return &body_; }

    void stick(Body* b);
    void unstick();
    
    State state() const { return state_; }

    static const num radius;
private:
    dJointID hinge_;
    Circle geom_;
    Body body_;
    State state_;
};

class Rope : public Object {
public:
    Rope(Object* obja, Body* bodya,
         Object* objb, Body* bodyb);

    ~Rope();
    
    void mark() {
        LEVEL->manager->mark(obja_);
        LEVEL->manager->mark(objb_);
    }

    void draw();

    num angle() {
        vec posa = proxya_.position();
        vec posb = proxyb_.position();
        return atan2(posb.y - posa.y, posb.x - posa.x);
    }

    void select()   { selected_ = true; }
    void deselect() { selected_ = false; }

    bool visible() const { return false; }
    
    void lengthen(num amt);
    
private:
    dJointID rope_;
    dJointID hingea_;
    dJointID hingeb_;
    Body proxya_;
    Body proxyb_;
    num ext_;
    num base_ext_;

    Object* obja_;
    Object* objb_;

    bool selected_;
};

bool collide(Object* a, Object* b, bool swap = true);

#endif
