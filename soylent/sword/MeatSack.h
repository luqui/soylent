#ifndef __MEATSACK_H__
#define __MEATSACK_H__

#include "physics.h"
#include "DrawQueue.h"
#include <iostream>

const double MEATSACK_FATNESS = 0.7;
const double MEATSACK_HEIGHT  = 2;
const double MEATSACK_MASS    = 60;

class MeatSack : public Body, public Geom, public Drawable
{
public:
    MeatSack() 
        : Geom(dCreateBox(0, MEATSACK_FATNESS, MEATSACK_HEIGHT, MEATSACK_FATNESS)), 
		hitcountdown_(0)
    {
        dGeomSetBody(geomid_, bodyid_);

        dMass mass;
        dMassSetBoxTotal(&mass, MEATSACK_MASS, 
                MEATSACK_FATNESS, MEATSACK_HEIGHT, MEATSACK_FATNESS);
        dBodySetMass(bodyid_, &mass);

		deaths_ = 0;
		touches_ = 0;
    }

    void set_position(vec3 pos) {
        dBodySetPosition(bodyid_, pos.x, pos.y, pos.z);
    }
    void apply_force(vec3 force) {
        dBodyAddForceAtRelPos(bodyid_, 
                              force.x, force.y, force.z,
                              get_position().x, get_position().y, get_position().z);
    }

	int get_hit_countdown() { return hitcountdown_; }
	bool get_fatal() const { return fatal_; }

	void set_hit_countdown(int count, bool fatal) 
	{ 
		// previous fatal hits override new non-fatal ones.
		if (fatal_) 
			return; 

		// repeated touches while still 'touched' don't count in the total.
		if (fatal)
			++deaths_;
		else if (hitcountdown_ == 0)
			++touches_;

		hitcountdown_ = count; 
		fatal_ = fatal;
	}

    void draw_delayed() const {
        vec3 pos = get_position();
        Quaternion rot = get_rotation();
        vec3 rot_axis = rot.get_axis();

        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(180 / M_PI * rot.get_angle(), rot_axis.x, rot_axis.y, rot_axis.z);

        const double f = MEATSACK_FATNESS/2;
        const double h = MEATSACK_HEIGHT/2;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (hitcountdown_ > 0)
		{
			if (fatal_)
				glColor4f(1,0,0,0.3);
			else
				glColor4f(1,1,0,0.3);
		}
		else
			glColor4f(1,1,1,0.3);

        glBegin(GL_QUADS);
            glNormal3f( 0,  0, -1);
            glVertex3f(-f, -h, -f);
            glVertex3f(-f,  h, -f);
            glVertex3f( f,  h, -f);
            glVertex3f( f, -h, -f);

            glNormal3f( 0,  0,  1);
            glVertex3f(-f, -h,  f);
            glVertex3f(-f,  h,  f);
            glVertex3f( f,  h,  f);
            glVertex3f( f, -h,  f);
            
            glNormal3f( 0, -1,  0);
            glVertex3f(-f, -h, -f);
            glVertex3f(-f, -h,  f);
            glVertex3f( f, -h,  f);
            glVertex3f( f, -h, -f);
            
            glNormal3f( 0,  1,  0);
            glVertex3f(-f,  h, -f);
            glVertex3f(-f,  h,  f);
            glVertex3f( f,  h,  f);
            glVertex3f( f,  h, -f);
            
            glNormal3f(-1,  0,  0);
            glVertex3f(-f, -h, -f);
            glVertex3f(-f,  h, -f);
            glVertex3f(-f,  h,  f);
            glVertex3f(-f, -h,  f);
            
            glNormal3f( 1,  0,  0);
            glVertex3f( f, -h, -f);
            glVertex3f( f,  h, -f);
            glVertex3f( f,  h,  f);
            glVertex3f( f, -h,  f);
        glEnd();
        glDisable(GL_BLEND);
        
        glPopMatrix();
    }

    void step() {
        set_angular_vel(vec3(0,0,0));

		if (hitcountdown_)
			--hitcountdown_;
		else
			fatal_ = false;
    }

	int get_deaths() const { return deaths_; }
	int get_touches() const { return touches_; }
	void set_deaths(int i) { deaths_ = i; }
	void set_touches(int i) { touches_ = i; }

private:
	int hitcountdown_;
	bool fatal_;

	int deaths_;
	int touches_;
};

#endif
