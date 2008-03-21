#ifndef __SWORD_H__
#define __SWORD_H__

#include "physics.h"

#include <soy/init.h>
#include <soy/util.h>
#include <soy/ptr.h>
#include <GL/gl.h>
#include <GL/glu.h>

// Sword is aligned along the body's z axis.
const vec3 forehand_pos(0,  0, -0.4);
const vec3 backhand_pos(0, 0, -0.55);
const double SWORD_RADIUS = 0.05;
const double SWORD_GUARD_RADIUS = 0.2;
const double SWORD_GUARD_LENGTH = 0.05;
const double SWORD_HILT_LENGTH = 0.3;
const double SWORD_LENGTH = 1.5;		// overall length, blade + guard + hilt.
const double SWORD_BLADE_LENGTH = SWORD_LENGTH - SWORD_GUARD_LENGTH - SWORD_HILT_LENGTH;
const double SWORD_MASS = 10.2;

// Change this?
// const double HANDLE_RATIO = 1/4.0;

class Sword : public Body, public Geom
{
public:
    Sword() 
		// 2cm radius, 1.2 meters long
        : Geom(dCreateBox(0, SWORD_RADIUS * 2, SWORD_RADIUS * 2, SWORD_BLADE_LENGTH)),  
		hilt_(dCreateBox(0, SWORD_RADIUS * 2, SWORD_RADIUS * 2, SWORD_HILT_LENGTH)),
		guard_(dCreateBox(0, SWORD_GUARD_RADIUS * 2, SWORD_GUARD_RADIUS * 2, SWORD_GUARD_LENGTH))
    {

		dGeomSetBody(geomid_, bodyid_);
		dGeomSetBody(hilt_.get_geomid(), bodyid_);
		dGeomSetBody(guard_.get_geomid(), bodyid_);

		// Stack the geoms so that the center of the sword as a whole is at the center.
		dGeomSetOffsetPosition (geomid_, 0, 0, - SWORD_LENGTH / 2 + SWORD_BLADE_LENGTH / 2 + SWORD_HILT_LENGTH + SWORD_GUARD_LENGTH);
		dGeomSetOffsetPosition (guard_.get_geomid(), 0, 0, -SWORD_LENGTH / 2 + SWORD_HILT_LENGTH + SWORD_GUARD_LENGTH / 2);
		dGeomSetOffsetPosition (hilt_.get_geomid(), 0, 0, -SWORD_LENGTH / 2 + SWORD_HILT_LENGTH / 2);

		// Change balance even more?

		double handleRatio = SWORD_HILT_LENGTH / SWORD_LENGTH;

		dMass blademass;
        dMassSetCappedCylinderTotal(&blademass, SWORD_MASS/2, 3, SWORD_RADIUS, (1-handleRatio) * SWORD_LENGTH);
        dMassTranslate(&blademass, 0, 0, ((1-handleRatio) - 0.5) * SWORD_LENGTH);

        dMass handlemass;
        dMassSetCappedCylinderTotal(&handlemass, SWORD_MASS/2, 3, SWORD_RADIUS, handleRatio * SWORD_LENGTH);
        dMassTranslate(&handlemass, 0, 0, (handleRatio - 0.5) * SWORD_LENGTH);

        dMassAdd(&handlemass, &blademass);
        dBodySetMass(bodyid_, &handlemass);
    }

    void set_position(vec3 pos) {
        dBodySetPosition(bodyid_, pos.x, pos.y, pos.z);
    }
    void set_orientation(const Quaternion &q) {
		dBodySetQuaternion(bodyid_, q.get_dQuaternion());
    }
    
    void apply_force_forehand(vec3 force) {
        dBodyAddForceAtRelPos(bodyid_, 
                              force.x, force.y, force.z,
                              forehand_pos.x, forehand_pos.y, forehand_pos.z);
    }

    void apply_force_backhand(vec3 force) {
        dBodyAddForceAtRelPos(bodyid_, 
                              force.x, force.y, force.z,
                              backhand_pos.x, backhand_pos.y, backhand_pos.z);
    }

    vec3 get_tip_position() const {
        return get_position() + get_rotation().rotate(vec3(0,0,SWORD_LENGTH/2));
    }

    vec3 get_tip_velocity() const {
        return get_velocity() + get_angular_vel() % (get_tip_position() - get_position());
    }
    
    vec3 get_handle_position() const {
        return get_position() + get_rotation().rotate(vec3(0,0,-SWORD_LENGTH/2));
    }

    vec3 get_handle_velocity() const {
        return get_velocity() + get_angular_vel() % (get_handle_position() - get_position());
    }

    double get_mass() const {
        return SWORD_MASS;
    }

    void draw() const {
        vec3 pos = get_position();
        Quaternion rot = get_rotation();
        vec3 rot_axis = rot.get_axis();

        
        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(180 / M_PI * rot.get_angle(), rot_axis.x, rot_axis.y, rot_axis.z);

		// draw grip.
        glColor3f(0.4, 0.2, 0.0);
        draw_shaft(-SWORD_LENGTH/2, -SWORD_LENGTH/2 + SWORD_HILT_LENGTH, SWORD_RADIUS);
		// draw the guard.
        // glColor3f(0.4, 0.2, 0.0);
		GLUquadric *lGluq = gluNewQuadric();
        glPushMatrix();
        glTranslatef(.0, .0, -SWORD_LENGTH/2 + SWORD_HILT_LENGTH + SWORD_GUARD_LENGTH / 2.0);
		// gluCylinder(lGluq, SWORD_GUARD_RADIUS, SWORD_GUARD_RADIUS, SWORD_GUARD_LENGTH, 100, 1);
		drawBox(SWORD_GUARD_RADIUS, SWORD_GUARD_RADIUS, SWORD_GUARD_LENGTH);
		glPopMatrix();
        //draw_shaft(-SWORD_LENGTH/2 + SWORD_HILT_LENGTH, -SWORD_LENGTH/2 + SWORD_HILT_LENGTH + SWORD_GUARD_LENGTH, SWORD_GUARD_RADIUS);
		// draw blade.
        glColor3f(0.8, 0.8, 0.8);
        draw_shaft(-SWORD_LENGTH/2 + SWORD_HILT_LENGTH + SWORD_GUARD_LENGTH, SWORD_LENGTH/2, SWORD_RADIUS);
        
        glPopMatrix();
    }
    
    void step() {
        cancel_angular_velocity(vec3(0,0,1));
    }

	static void drawBox(const double &x, const double &y, const double &z)
	{
		glBegin(GL_QUADS);
            glNormal3f( 0,  0, -1);
            glVertex3f(-x, -y, -z);
            glVertex3f(-x,  y, -z);
            glVertex3f( x,  y, -z);
            glVertex3f( x, -y, -z);

            glNormal3f( 0,  0,  1);
            glVertex3f(-x, -y,  z);
            glVertex3f(-x,  y,  z);
            glVertex3f( x,  y,  z);
            glVertex3f( x, -y,  z);
            
            glNormal3f( 0, -1,  0);
            glVertex3f(-x, -y, -z);
            glVertex3f(-x, -y,  z);
            glVertex3f( x, -y,  z);
            glVertex3f( x, -y, -z);
            
            glNormal3f( 0,  1,  0);
            glVertex3f(-x,  y, -z);
            glVertex3f(-x,  y,  z);
            glVertex3f( x,  y,  z);
            glVertex3f( x,  y, -z);
            
            glNormal3f(-1,  0,  0);
            glVertex3f(-x, -y, -z);
            glVertex3f(-x,  y, -z);
            glVertex3f(-x,  y,  z);
            glVertex3f(-x, -y,  z);
            
            glNormal3f( 1,  0,  0);
            glVertex3f( x, -y, -z);
            glVertex3f( x,  y, -z);
            glVertex3f( x,  y,  z);
            glVertex3f( x, -y,  z);
        glEnd();
	}

private:

	// Hilt and guard geoms.  The sword object itself is the blade.
	Geom hilt_;
	Geom guard_;

    void draw_shaft(double z1, double z2, double radius) const {
        const double sqrt5 = sqrt(5.0);
        glBegin(GL_QUADS);
            glNormal3f(-1/sqrt5, -2/sqrt5, 0);
            glVertex3f(-radius, 0,               z1);
            glVertex3f(-radius, 0,               z2);
            glVertex3f(0,             -radius/2, z2);
            glVertex3f(0,             -radius/2, z1);

            glNormal3f(1/sqrt5, -2/sqrt5, 0);
            glVertex3f( radius, 0,               z1);
            glVertex3f( radius, 0,               z2);
            glVertex3f(0,             -radius/2, z2);
            glVertex3f(0,             -radius/2, z1);
            
            glNormal3f(1/sqrt5, 2/sqrt5, 0);
            glVertex3f( radius, 0,               z1);
            glVertex3f( radius, 0,               z2);
            glVertex3f(0,              radius/2, z2);
            glVertex3f(0,              radius/2, z1);
            
            glNormal3f(-1/sqrt5, 2/sqrt5, 0);
            glVertex3f(-radius, 0,               z1);
            glVertex3f(-radius, 0,               z2);
            glVertex3f(0,              radius/2, z2);
            glVertex3f(0,              radius/2, z1);
        glEnd();
    }
};

#endif
