#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <ode/ode.h>
#include <soy/vec2.h>
#include <cmath>

extern const double STEP;
extern dWorldID WORLD;
extern dJointGroupID CONTACTS;
extern dSpaceID SPACE;

inline double sign(double in)
{
    return copysign(1,in);
}

inline double quaternion_to_rotation(const dQuaternion q)
{
    return 2*acos(q[0]) * sign(q[3]);
}

inline double dbody_get_rotation(dBodyID body)
{
    const dReal* q = dBodyGetQuaternion(body);
    return quaternion_to_rotation(q);
}

inline vec2 dbody_get_position(dBodyID body)
{
    const dReal* pos = dBodyGetPosition(body);
    return vec2(pos[0], pos[1]);
}

inline vec2 dbody_get_velocity(dBodyID body)
{
    const dReal* pos = dBodyGetLinearVel(body);
    return vec2(pos[0], pos[1]);
}

inline double dbody_get_angular_vel(dBodyID body)
{
    const dReal* pos = dBodyGetAngularVel(body);
    return pos[2];
}

inline void apply_damping(dBodyID body, double linear, double angular)
{
    vec2 vel = dbody_get_velocity(body);
    vec2 lindamp = -linear * vel;
    double ang = dbody_get_angular_vel(body);
    double angdamp = -angular * ang;
    
    dBodyAddForce(body, lindamp.x, lindamp.y, 0);
    dBodyAddTorque(body, 0, 0, angdamp);
}

#endif
