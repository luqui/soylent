#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <ode/ode.h>
#include <soy/vec2.h>

const double DT = 1/60.0;

class Physics
{
public:
	Physics() {
		world = dWorldCreate();
		contacts = dJointGroupCreate(0);
		space = dHashSpaceCreate(0);

		activate();
	}

	void step() {
		dWorldQuickStep(world, DT);
		dJointGroupEmpty(contacts);
	}
	
	void activate();

	dWorldID world;
	dJointGroupID contacts;
	dSpaceID space;
};

extern Physics* PHYS;

class Geom
{
public:
	virtual ~Geom() {
		dGeomDestroy(geom);
	}

	const dGeomID geom;

protected:
	Geom(dGeomID id) : geom(id) {
		dGeomSetData(geom, this);
	}
	
	vec2 geom_pos() const {
		const dReal* pos = dGeomGetPosition(geom);
		return vec2(pos[0], pos[1]);
	}

private:
	// no copying
	Geom(const Geom&);
	Geom& operator = (const Geom&);
};


class Body
{
public:
	virtual ~Body() {
		dBodyDestroy(body);
		dJointDestroy(plane2d_joint_);
	}
	
	const dBodyID body;

protected:
	Body() 
		: body(dBodyCreate(PHYS->world))
		, plane2d_joint_(dJointCreatePlane2D(PHYS->world, 0))
	{
		dBodySetData(body, this);
		dJointAttach(plane2d_joint_, body, 0);
	}

	vec2 body_pos() const {
		const dReal* pos = dBodyGetPosition(body);
		return vec2(pos[0], pos[1]);
	}

private:
	const dJointID plane2d_joint_;

	// no copying
	Body(const Body&);
	Body& operator = (const Body&);
};

#endif
