#ifndef __DUELMODE_H__
#define __DUELMODE_H__

#include "Arena.h"
#include "GameMode.h"
#include "Sword.h"
#include "SwordController.h"
#include "Fighter.h"
#include "audio.h"
#include "config.h"
#include <stack>
#include <utility>
#include <gl/glut.h>
#include <sstream>

void collision_callback(void* data, dGeomID ga, dGeomID gb);

class DuelMode : public GameMode
{
public:
    DuelMode(const NVThing &aNvtSPPrefs) 
		: player1_(StanceBank::SMakeStance(aNvtSPPrefs, "Player1Stance"), vec3(0, MEATSACK_HEIGHT / 2, 0), vec3(1,0,0))
        , player2_(StanceBank::SMakeStance(aNvtSPPrefs, "Player2Stance"), vec3(4, MEATSACK_HEIGHT / 2, 0), vec3(-1,0,0))
        //: player1_(vec3(.9, MEATSACK_HEIGHT / 2, 0), vec3(1,0,0))
        //, player2_(vec3(6.0, MEATSACK_HEIGHT / 2, 0), vec3(-1,0,0))
        //: player1_(vec3(2, MEATSACK_HEIGHT / 2, -4), vec3(0,0,1))
        //, player2_(vec3(2.0, MEATSACK_HEIGHT / 2, 4), vec3(0,0,-1))
    { }

	enum Perspective
	{
		PPlayer1,
		PPlayer2,
		PGlobal
	};
    
    void draw(vec3 viewfrom, Perspective P) const {
        glPushMatrix();
        glLoadIdentity();

		// 'append' the viewfrom vector onto the meatsack
		// position and orientation.
		const Fighter *lF;

		switch (P)
		{
		case PPlayer1:
			lF = &player1_;
			break;
		case PPlayer2:
			lF = &player2_;
			break;
		case PGlobal:
			lF = 0;
			break;
		default:
            DIE("Only 2 players!");
		}

		vec3 direction;
		if (lF)
		{
			lF->get_meatsack_pos();
			viewfrom = lF->get_meatsack_rot().rotate(viewfrom);
			viewfrom += lF->get_meatsack_pos();
			direction = vec3(0, 0, -5);
			direction = lF->get_meatsack_rot().rotate(direction);
			direction += lF->get_meatsack_pos();
		}
		else
		{
			// Look at the point in between the two players.
			direction = player1_.get_meatsack_pos() + player2_.get_meatsack_pos();
			direction *= .5;
			// view_from is relative to this position, and magnitude is 
			// enough to see both players at once.
			viewfrom = player1_.get_meatsack_pos();
			viewfrom -= player2_.get_meatsack_pos();
			// double mag = viewfrom.norm();
			double z = viewfrom.x;
			viewfrom.x = -viewfrom.z;
			viewfrom.z = z;
			viewfrom.y = 2.0;
			viewfrom += direction;
		}

		gluLookAt(viewfrom.x,  viewfrom.y,  viewfrom.z,
				direction.x,   direction.y,  direction.z,
				0,   1,  0);

		// Now draw the objs.
        arena_.draw();
        player1_.draw();
        player2_.draw();
        
        glPopMatrix();
    }

	virtual void draw_scores()
	{
		glMatrixMode(GL_PROJECTION);
        glPushMatrix();

		glLoadIdentity();
		gluOrtho2D(-INIT.width()/2,INIT.width()/2,INIT.height()/2,-INIT.height()/2);

		// upper left is (0, 0)
		glTranslatef(-INIT.width()/2, -INIT.height()/2, 0);

		glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
		glLoadIdentity();

		// Draw scores.
		std::ostringstream lOss;
		lOss << "Player1: " << player2_.get_meatsack()->get_deaths() << " kills, " << player2_.get_meatsack()->get_touches() << " touches";

		glRasterPos3d(0, 15, 0);

        for (size_t i = 0; i < lOss.str().size(); i++) 
		{
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, lOss.str()[i]);
        }

		lOss.seekp(0);
		lOss << "Player2: " << player1_.get_meatsack()->get_deaths() << " kills, " << player1_.get_meatsack()->get_touches() << " touches";

		glRasterPos3d(INIT.width() - lOss.str().size() * 9, 15, 0);
        for (size_t i = 0; i < lOss.str().size(); i++) 
		{
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, lOss.str()[i]);
        }



		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
	    
	}

    void step() 
	{
        dSpaceCollide(COLLIDE_SPACE, this, &collision_callback);
        dWorldStep(WORLD, DT);
        dJointGroupEmpty(CONTACT_JOINT_GROUP);

		// Make fighters always face each other.
		vec3 v12(player2_.get_meatsack_pos());
		v12 -= player1_.get_meatsack_pos();
		v12 = ~v12;

		player1_.set_facing_direction(v12);
		player2_.set_facing_direction(-v12);

		// Do fighter adjustments.
        player1_.step();
        player2_.step();
    }
	
    virtual void check_collision(dGeomID ga, dGeomID gb) {
		if (dGeomIsSpace(ga) || dGeomIsSpace(gb)) {
			// collide a space with the other object
			dSpaceCollide2(ga, gb, this, &collision_callback);
			// check for collisions internally to the space
			if (dGeomIsSpace(ga)) dSpaceCollide(dSpaceID(ga), this, &collision_callback);
			if (dGeomIsSpace(gb)) dSpaceCollide(dSpaceID(gb), this, &collision_callback);
		}
		else {
			const int max_contacts = 16;
			dContactGeom contacts[max_contacts];
			int ncontacts = dCollide(ga, gb, max_contacts, contacts, sizeof(dContactGeom));

			if (ncontacts > 0) {
                
				Geom* ba = (Geom*)dGeomGetData(ga);
				Geom* bb = (Geom*)dGeomGetData(gb);
                // do game logic...

				Sword *lS;
				MeatSack *lM;
                
				if (dynamic_cast<Sword*>(ba) && dynamic_cast<Sword*>(bb)) {
					SOUND.play("sword_clang.wav");
				} 
				else if ((lS = dynamic_cast<Sword*>(ba)) && (lM = dynamic_cast<MeatSack*>(bb)) ||
						 (lS = dynamic_cast<Sword*>(bb)) && (lM = dynamic_cast<MeatSack*>(ba)))
				{
					// If the other player is dead, this strike won't count.
					// This also disallows self-swording when the opponent is dead.
					if ((lM == player1_.get_meatsack() && !player2_.get_meatsack()->get_fatal()) ||
						(lM == player2_.get_meatsack() && !player1_.get_meatsack()->get_fatal()))
					{
						dVector3 velS, velM;
						dBodyGetPointVel(lS->get_bodyid(), contacts[0].pos[0], contacts[0].pos[1], contacts[0].pos[2], velS);
						dBodyGetPointVel(lM->get_bodyid(), contacts[0].pos[0], contacts[0].pos[1], contacts[0].pos[2], velM);

						vec3 lV(velS[0], velS[1], velS[2]);
						lV -= vec3(velM[0], velM[1], velM[2]);
						double v = lV.norm();

						if (v > 3.0)
							lM->set_hit_countdown(500, true);
						else
							lM->set_hit_countdown(250, false);

						OnMeatSackSworded(lM, lS);
					}
				}
			}

			for (int i = 0; i < ncontacts; i++) {
				dContact contact;
				contact.geom = contacts[i];
				contact.surface.mode = dContactBounce;
				contact.surface.mu = 0.3;
				contact.surface.bounce = 0.3;
				contact.surface.bounce_vel = 1;

				dJointID ctct = dJointCreateContact(WORLD, CONTACT_JOINT_GROUP, &contact);
				dJointAttach(ctct, dGeomGetBody(ga), dGeomGetBody(gb));
			}
		}
	}

    Fighter player1_;
    Fighter player2_;

protected:
	virtual void OnMeatSackSworded(MeatSack *aM, Sword* aS) {}
private:
    Arena arena_;

    std::stack<Stance*> restore_stance_p1_;
    std::stack<Stance*> restore_stance_p2_;
};


inline void collision_callback(void* data, dGeomID ga, dGeomID gb) {
    ((DuelMode*)data)->check_collision(ga, gb);
}

#endif
