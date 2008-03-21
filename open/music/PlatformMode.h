#ifndef __PLATFORMMODE_H__
#define __PLATFORMMODE_H__

#include <memory>
#include <soy/Viewport.h>
#include <soy/util.h>
#include <list>
#include <set>

#include "GameMode.h"
#include "physics.h"
#include "Player.h"
#include "Platform.h"
#include "Metronome.h"
#include "Sound.h"

void collision_callback(void* data, dGeomID ga, dGeomID gb);

class PlatformMode : public GameMode
{
public:
	PlatformMode() : view_(vec2(20, 16), vec2(40, 32)), metro_(80), synth_("musicgame.sf2") {
		dWorldSetGravity(PHYS->world, 0, -30, 0);
		SynthTrack* organ = synth_.new_track();
		organist_ = new Technique(&metro_, organ);
		organ->set_patch(0, 13);
		
		SynthTrack* drums = synth_.new_track();
		drummer_ = new Technique(&metro_, drums);
		drums->set_patch(128,0);
		

		for (int i = 1; i < 200; i++) {
			//plats_.push_back(new FixedPlatform(vec2(i*5 + randrange(-2, 2), 16 + randrange(-10, 10)), vec2(randrange(1,4), 1)));
			double quant = 1.0 / pow(2, int(randrange(0, 4)));
			plats_.push_back(new RegenPlatform(organist_, &metro_, Beat(quant, int(randrange(0, 16)) * quant),
					vec2(i*2 + randrange(-2, 2), 16 + randrange(-10, 10)), vec2(4*quant, 1)));
			if (i % 4 == 0) {
				plats_.push_back(new FixedPlatform(organist_,
						vec2(i*2, 0), vec2(7.75, 1)));
			}
		}
	}
	virtual ~PlatformMode() { }

	void step() {
		view_.center += vec2(5,0) * DT;
		dSpaceCollide(PHYS->space, this, &collision_callback);
		PHYS->step();
		player_.step();
		metro_.step();
		if (metro_.quantum(Beat(1/16.0))) {
			for (soundq_t::iterator i = soundq_.begin(); i != soundq_.end(); i++) {
				(*i)->play();
				delete *i;
			}
			soundq_.clear();
		}
		organist_->step();

		if (metro_.quantum(Beat(1/4.0))) {
			drummer_->play_max_dur(54, 96, 1/8.0);
		}

		for (plats_t::iterator i = plats_.begin(); i != plats_.end(); ++i) {
			(*i)->step();
		}
	}

	void draw() const {
		view_.activate();
		for (plats_t::const_iterator i = plats_.begin(); i != plats_.end(); ++i) {
			(*i)->draw();
		}
		player_.draw();
	}

	bool events(SDL_Event* e) {
		if(player_.events(e)) return true;
		return false;
	}

	virtual void collide(Geom* oa, Geom* ob, dContactGeom* contacts, int ncontacts) {
		SoundEvent* e = 0;
		if (Platform* p = dynamic_cast<Platform*>(oa)) {
			if (!p->collide()) return;
			e = p->sound();
		}
		if (Platform* p = dynamic_cast<Platform*>(ob)) {
			if (!p->collide()) return;
			e = p->sound();
		}
		
		if (e) {
			soundq_t::iterator i = soundq_.find(e);
			if (i == soundq_.end()) {
				soundq_.insert(e);
			}
			else {
				delete e;
			}
		}
		
		float bouncefactor = 0.75;
		if (Player* p = dynamic_cast<Player*>(oa)) {
			if (p->powerbounce()) {
				bouncefactor = 1.3;
				dBodyAddForce(p->body, 0, 1/DT, 0);
			}
		}
		if (Player* p = dynamic_cast<Player*>(ob)) {
			if (p->powerbounce()) {
				bouncefactor = 1.3;
				dBodyAddForce(p->body, 0, 1/DT, 0);
			}
		}
		for (int i = 0; i < ncontacts; i++) {
			dContact* contact = new dContact;
			contact->surface.mode = dContactBounce;
			contact->surface.mu = 0;
			contact->surface.bounce = bouncefactor;
			contact->surface.bounce_vel = 0;
			contact->geom = contacts[i];
			
			dJointID ctct = dJointCreateContact(PHYS->world, PHYS->contacts, contact);
			dJointAttach(ctct, dGeomGetBody(oa->geom), dGeomGetBody(ob->geom));
			delete contact;
		}
	}

protected:
	Metronome metro_;
	Physics phys_;
	Player player_;

	FluidSynth synth_;

	Technique* organist_;
	Technique* drummer_;

	typedef std::list<Platform*> plats_t;
	plats_t plats_;

	typedef std::set<SoundEvent*, SoundEvent::Comparator> soundq_t;
	soundq_t soundq_;

	Viewport view_;
};

inline void collision_callback(void* data, dGeomID ga, dGeomID gb) 
{
	if (dGeomIsSpace(ga) || dGeomIsSpace(gb)) {
		dSpaceCollide2(ga, gb, data, &collision_callback);

		if (dGeomIsSpace(ga)) {
			dSpaceCollide(dSpaceID(ga), data, &collision_callback);
		}
		if (dGeomIsSpace(gb)) {
			dSpaceCollide(dSpaceID(gb), data, &collision_callback);
		}
	}
	else {
		const int max_contacts = 1;
		dContactGeom contacts[max_contacts];
		int ncontacts = dCollide(ga, gb, max_contacts, contacts, sizeof(dContactGeom));

		if (ncontacts > 0) {
			Geom* oa = (Geom*)dGeomGetData(ga);
			Geom* ob = (Geom*)dGeomGetData(gb);
			
			((PlatformMode*)data)->collide(oa, ob, contacts, ncontacts);
		}
	}
}

#endif
