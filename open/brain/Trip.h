#ifndef __TRIP_H__
#define __TRIP_H__

class Trip {
public:
    virtual ~Trip() { }
    virtual bool done() const = 0;
    virtual void step() { }
    virtual void draw() const { }
};

#endif
