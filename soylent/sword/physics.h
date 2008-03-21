#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <ode/ode.h>
#include <soy/vec3.h>
#include <cmath>

const int MULTISTEP = 5;
const double DT = 1/(60.0 * MULTISTEP);
extern dWorldID WORLD;
extern dSpaceID COLLIDE_SPACE;
extern dJointGroupID CONTACT_JOINT_GROUP;

inline vec3 dVector_to_vec(const dReal* vec) 
{
    return vec3(vec[0], vec[1], vec[2]);
}

class Quaternion
{
public:
    Quaternion() {
        dQSetIdentity(q_);
    }
    
    Quaternion(const dQuaternion in) {
        q_[0] = in[0];
        q_[1] = in[1];
        q_[2] = in[2];
        q_[3] = in[3];
    }

    const dReal* get_dQuaternion() const {
        return q_;
    }

    double get_angle() const {
        return 2 * acos(q_[0]);
    }

    vec3 get_axis() const {
        dReal sina = sqrt(1 - q_[0]*q_[0]);
        if (fabs(sina) < 0.0005) { sina = 1; }  // no dividing by almost-0
        return vec3(q_[1], q_[2], q_[3]) / sina;
    }

    vec3 rotate(vec3 v) const {
        dQuaternion vq = { 0, v.x, v.y, v.z };
        dQuaternion right;
        dQMultiply2(right, vq, q_);   // vq * q^-1
        
        dQuaternion result;
        dQMultiply0(result, q_, right);  // q * (vq * q^-1)

        return vec3(result[1], result[2], result[3]);
    }

	void calcrotation(const vec3 &NormStartVec, const vec3 &NormEndVec)
	{
		float	CosTheta	= NormStartVec * NormEndVec;

		//--------------------------------------------------------------------------------
		// Now use half angle formulas to get sine(theta/2) and cos(theta/2) to create the quat with
		// Cos (t/2) = SqRoot(   (Cos(t) + 1) / 2   )
		//--------------------------------------------------------------------------------

		float CosTd2Sq = (CosTheta + 1.0f) * 0.5f;
		float CosTd2 = sqrt(  CosTd2Sq );
		float SinTd2 = sqrt( 1.0f - CosTd2Sq);

		//--------------------------------------------------------------------------------
		// Get the rotation axis
		//--------------------------------------------------------------------------------

		vec3 RotAxis;

		//--------------------------------------------------------------------------------
		// Special case, vectors in opposite directions, can't directly create a rotation vector
		// (not a problem with both facing same direction, sint/2 will be 0, creating unity Quat.)
		//--------------------------------------------------------------------------------

		if (CosTheta < -0.999f)								//very nearly opposite
		{
			vec3 NonParraVec(0.0f,1.0f,0.0f);				//Get a vector thats not aligned with the startvec
			
			RotAxis = NonParraVec % NormStartVec;

			if (RotAxis.norm2() < 0.001f)		//Still aligned
			{
				RotAxis = vec3(1, 0, 0) % NormStartVec;
				// DBG(if (RotAxis.magnitudeSquared() < 0.001f) ASSERT("Ehh? should have found a rot axis by now");)
			}
		}
		else
		{
			RotAxis = NormStartVec % NormEndVec;
		}

		if (SinTd2 < 0.0004f)								//Vectors lined up, no need for an axis
		{
			RotAxis = vec3(0, 0, 0);				//No axis needed if no rotation
		}
		else
		{
			RotAxis = ~RotAxis;
		}
		//--------------------------------------------------------------------------------
		// Now we have all the info, create the quaterion
		//--------------------------------------------------------------------------------
		q_[0] = CosTd2;
		q_[1] = RotAxis.x;
		q_[2] = RotAxis.y;
		q_[3] = RotAxis.z;
		q_[1] *= SinTd2;
		q_[2] *= SinTd2;
		q_[3] *= SinTd2;
	}
private:
    dQuaternion q_;
};

class Body
{
public:
    Body() 
        : bodyid_(dBodyCreate(WORLD)) 
    { }

    virtual ~Body() {
        dBodyDestroy(bodyid_);
    }

    virtual vec3 get_position() const {
        return dVector_to_vec(dBodyGetPosition(bodyid_));
    }

    virtual Quaternion get_rotation() const {
        return Quaternion(dBodyGetQuaternion(bodyid_));
    }

    virtual vec3 get_velocity() const {
        return dVector_to_vec(dBodyGetLinearVel(bodyid_));
    }

    virtual vec3 get_angular_vel() const {
        return dVector_to_vec(dBodyGetAngularVel(bodyid_));
    }

    virtual void set_angular_vel(vec3 avel) {
        dBodySetAngularVel(bodyid_, avel.x, avel.y, avel.z);
    }

    virtual void apply_force(vec3 force) {
        dBodyAddForce(bodyid_, force.x, force.y, force.z);
    }

    virtual void apply_torque(vec3 torque) {
        dBodyAddTorque(bodyid_, torque.x, torque.y, torque.z);
    }

	virtual double get_mass() const
	{
		dMass daMass;
		dBodyGetMass(bodyid_, &daMass);
		return daMass.mass;
	}

	const dBodyID& get_bodyid() const { return bodyid_; }

    // removes the angular velocity component along the axis 'axis'
    inline void cancel_angular_velocity(vec3 axis) 
    {
        dVector3 majordv;
        dBodyGetRelPointPos(bodyid_, axis.x, axis.y, axis.z, majordv);
        vec3 major = vec3(majordv[0], majordv[1], majordv[2]) - get_position();
        vec3 angvel = get_angular_vel();
        angvel -= (angvel * major) * major;
        set_angular_vel(angvel);
    }

protected:
    dBodyID bodyid_;

private:
    // It is not safe to copy Bodys
    Body(const Body&);
    Body& operator = (const Body&);
};


class Geom
{
public:
    Geom(dGeomID geom) 
        : geomid_(geom)
    {
        dSpaceAdd(COLLIDE_SPACE, geom);
        dGeomSetData(geomid_, this);
    }

    virtual ~Geom() {
        dGeomDestroy(geomid_);
    }

	const dGeomID& get_geomid() const { return geomid_; }

    
protected:
    dGeomID geomid_;

private:
    // It is not safe to copy Geoms
    Geom(const Geom&);
    Geom& operator = (const Geom&);
};


inline dGeomID create_plane_geom(vec3 pos, vec3 normal)
{
    // The plane is defined by: ax + by + cz = d
    // (a,b,c) is the normal, so:
    // d = a pos.x + b pos.y + c pos.z; i.e. d = normal * pos
    
    normal = ~normal;  // normalize
    return dCreatePlane(0, normal.x, normal.y, normal.z, normal * pos);
}


#endif
