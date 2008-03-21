#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "common.h"
#include "vec.h"
#include "level.h"

extern num STEP;
extern num OVERSTEP;

class Object;

class Body {
    public:
        Body() : owner_(0) {
            body_ = dBodyCreate(LEVEL->world);
            dBodySetData(body_, static_cast<void*>(this));
            plane2d_ = dJointCreatePlane2D(LEVEL->world, 0);
            dJointAttach(plane2d_, body_, 0);
        }
        
        ~Body() {
            dJointDestroy(plane2d_);
            dBodyDestroy(body_);
        }

        vec position() const {
            const dReal* ps = dBodyGetPosition(body_);
            vec vel = velocity();
            return vec(ps[0], ps[1]) + vel * OVERSTEP;  // CGL correction
        }

        void set_position(const vec& p) {
            dBodySetPosition(body_, p.x, p.y, 0);
        }

        vec velocity() const {
            const dReal* vs = dBodyGetLinearVel(body_);
            return vec(vs[0], vs[1]);
        }

        void set_velocity(const vec& v) {
            dBodySetLinearVel(body_, v.x, v.y, 0);
        }

        num angle() const {
            const dReal* q = dBodyGetQuaternion(body_);
            num omega = angular_velocity();
            return 2*acos(q[0]) * sign(q[3]) + omega * OVERSTEP;  // CGL correction
        }

        void set_angle(num theta) {
            dQuaternion q;
            dQFromAxisAndAngle(q, 0, 0, 1, theta);
            dBodySetQuaternion(body_, q);
        }

        num angular_velocity() const {
            const dReal* q = dBodyGetAngularVel(body_);
            return q[2];
        }

        void set_angular_velocity(num omega) {
            dBodySetAngularVel(body_, 0, 0, omega);
        }

        void apply_force(vec f, vec p) {
            dBodyAddForceAtPos(body_, f.x, f.y, 0, p.x, p.y, 0);
        }

        void set_mass(num mass, num radius = 1) {
            dMass dmass;
            dMassSetSphereTotal(&dmass, mass, radius);
            dBodySetMass(body_, &dmass);
        }

        dBodyID body_id() const { return body_; }

        void set_owner(Object* w) { owner_ = w; }

        Object* owner() const { return owner_; }

        static Body* from_body_id(dBodyID body) {
            if (body)
                return static_cast<Body*>(dBodyGetData(body));
            else
                return 0;
        }
        
    private:
        Object* owner_;
        dBodyID body_;
        dJointID plane2d_;
};

#endif
