#ifndef __TWEAK_H__
#define __TWEAK_H__

#include <list>

const int INITIAL_NODE_MIN = 1;
const int INITIAL_NODE_MAX = 10;
const double INITIAL_CONNECTION_PROBABILITY = 0.7;

const double CELL_MUTATION_RATE           = 0.1;

const double CONTENT_MUTATION_PROBABILITY = 0.5;
const double NEW_CONNECTION_PROBABILITY   = 0.4;
const double ALTER_TOP_PROBABILITY        = 0.1;
const double BREAK_CONNECTION_PROBABILITY = 0.1;
const double REPLACE_NODE_PROBABILITY     = 0.1;
const double DELETE_NODE_PROBABILITY      = 0.02;
const double ADD_NODE_SLOT_PROBABILITY    = 0.01;

const double PARTICLE_DAMPING             = 0.01;
const double CELL_DAMPING                 = 0.04;

const double MAX_VELOCITY_PER_ENERGY      = 1;
const double MOTION_MUTATION              = 0.1;
const double MOTION_MUTATION_RATE         = 0.5;
const double MOTION_ANGLE_MUTATION        = 0.1;
const double MOTION_ANGLE_MUTATION_RATE   = 0.05;

const double REPRODUCTION_FOOD_MUTATION_RATE   = 0.3;
const double REPRODUCTION_DIRECTION_MUTATION_RATE = 0.3;
const double REPRODUCTION_DIRECTION_MUTATION = 0.1;
const double REPRODUCTION_ANGLE_MUTATION_RATE  = 0.3;
const double REPRODUCTION_ANGLE_MUTATION  = 0.225;

const double EJECTOR_DIRECTION_MUTATION   = 0.1;

const int    RATION_MAX_INITIAL_CAPACITY  = 20;
const double RATION_MAX_INITIAL_TIME      = 10;
const double RATION_CAPACITY_MUTATION_RATE = 0.5;
const double RATION_TIME_MUTATION_RATE    = 0.5;
const double RATION_TIME_MUTATION         = 1;

const double INITIAL_FOODTIME             = 10;
const double FOODTIME_PER_ENERGY          = 5;
const double MAX_STORED_FOODTIME          = 31;

const double DT = 1/30.0;

const double PARTICLE_FIELD_BIN_SIZE      = 5;
const double PARTICLE_FIELD_ACTIVE_THRESHOLD = 0.01;


class SoyInit;        extern SoyInit INIT;
class ParticleField;  extern ParticleField PARTICLES;
class Viewport;       extern Viewport VIEWPORT;

class Cell;
typedef std::list<Cell*> cells_t;
extern cells_t CELLS;

#endif
