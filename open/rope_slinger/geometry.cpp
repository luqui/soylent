#include "geometry.h"
#include "objects.h"

static void near_callback(void* data, dGeomID ga, dGeomID gb) {
    if (dGeomIsSpace(ga) || dGeomIsSpace(gb)) {
        dSpaceCollide2(ga, gb, data, &near_callback);

        if (dGeomIsSpace(ga)) dSpaceCollide(reinterpret_cast<dSpaceID>(ga), data, &near_callback);
        if (dGeomIsSpace(gb)) dSpaceCollide(reinterpret_cast<dSpaceID>(gb), data, &near_callback);
    }
    else {
        dBodyID bodya = dGeomGetBody(ga);
        dBodyID bodyb = dGeomGetBody(gb);
        if (bodya == 0 && bodyb == 0) return;

        Object* obja = Geom::from_geom_id(ga)->owner();
        Object* objb = Geom::from_geom_id(gb)->owner();
        if (obja && objb && !collide(obja, objb)) return;
        
        dContactGeom geoms[16];
        int n_contacts = dCollide(ga, gb, 16, geoms, sizeof(dContactGeom));
        for (int c = 0; c < n_contacts; c++) {
            dContact contact;
            contact.surface.mode = dContactFDir1 | dContactBounce;
            contact.surface.mu = 0;
            contact.surface.bounce = 0.2;
            contact.surface.bounce_vel = 0.5;
            contact.fdir1[0] = geoms[c].normal[1];
            contact.fdir1[1] = -geoms[c].normal[0];
            contact.fdir1[2] = 0;
            contact.geom = geoms[c];

            dJointID joint = dJointCreateContact(LEVEL->world, LEVEL->contact_joints, &contact);
            dJointAttach(joint, bodya, bodyb);
        }
    }
}

void collide() {
    dSpaceCollide(LEVEL->collide_space, 0, &near_callback);
}
