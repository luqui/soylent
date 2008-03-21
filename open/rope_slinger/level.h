#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "common.h"

class Player;
class ObjectManager;

class Level {
public:
    Level()
        : left(FP_NAN), right(FP_NAN), top(FP_NAN), bottom(FP_NAN),
          p1(0), p2(0), player(0), manager(0), max_ropes(0),
          world(0), collide_space(0), contact_joints(0),
          frozen_(true), freeze_timer_(0)
    { }
    ~Level();
    
    void load_level(string file);

    void step();
    void draw();

    void freeze();
    void unfreeze();

    bool frozen() { return frozen_; }

    num left, right, top, bottom;
    Player* p1;
    Player* p2;
    Player* player;

    ObjectManager* manager;
    
    size_t max_ropes;
    
    num fire_velocity;
    
    dWorldID world;
    
    dSpaceID collide_space;
    dJointGroupID contact_joints;

private:
    void set_view();
    
    int stepct_;
    bool frozen_;
    num freeze_timer_;
    num wait_timer_;
    Player* next_player_;
};

extern Level* LEVEL;

#endif
