#include "objects.h"

void ObjectManager::mark(Object* o) {
    if (o->visible_) return;
    o->visible_ = true;
    
    if (gc_tail_) gc_tail_->gc_next_ = o;
    o->gc_next_ = 0;
    gc_tail_ = o;
}

void ObjectManager::sweep() {
    Object* cptr = 0;
    
    gc_tail_ = 0;
    for (set_t::iterator i = oset_.begin(); i != oset_.end(); ++i) {
        (*i)->visible_ = false;
        if ((*i)->visible()) {
            if (!cptr) cptr = *i;
            mark(*i);
        }
    }

    while (cptr) {
        cptr->mark();
        cptr = cptr->gc_next_;
    }

    int cleaned = 0;
    for (set_t::iterator i = oset_.begin(); i != oset_.end(); ) {
        Object* o = *i;
        if (!o->visible_) {
            set_t::iterator next = i;  ++next;
            oset_.erase(i);
            i = next;

            delete o;
            cleaned++;
        }
        else {
            ++i;
        }
    }

    if (cleaned) cout << "Cleaned up " << cleaned << " objects\n";
}

const num Spikey::radius = 0.3;

Spikey::Spikey(vec p, vec f)
    : hinge_(0), geom_(p, radius), state_(STICKY)
{
    body_.set_position(p);
    body_.set_owner(this);
    geom_.set_owner(this);
    geom_.attach(&body_);
    body_.set_mass(0.1, radius);

    body_.apply_force(f, p);
}

void Spikey::stick(Body* b) {
    if (hinge_) dJointDestroy(hinge_);
    hinge_ = dJointCreateFixed(LEVEL->world, 0);
    dJointAttach(hinge_, body_.body_id(), b ? b->body_id() : 0);
    dJointSetFixed(hinge_);
    state_ = STUCK;
}

void Spikey::unstick() {
    if (hinge_) {
        dJointDestroy(hinge_);
        hinge_ = 0;
    }
    state_ = UNSTUCK;
}

Rope::Rope(Object* obja, Body* bodya, Object* objb, Body* bodyb)
    : obja_(obja), objb_(objb), selected_(false)
{
    vec apos = bodya->position();
    proxya_.set_position(apos);
    proxya_.set_velocity(bodya->velocity());
    proxya_.set_mass(0.01, 1);
    hingea_ = dJointCreateHinge(LEVEL->world, 0);
    dJointAttach(hingea_, proxya_.body_id(), bodya->body_id());
    dJointSetHingeAxis(hingea_, 0, 0, 1);

    vec bpos = bodyb->position();
    proxyb_.set_position(bpos);
    proxyb_.set_velocity(bodyb->velocity());
    proxyb_.set_mass(0.01, 1);
    hingeb_ = dJointCreateHinge(LEVEL->world, 0);
    dJointAttach(hingeb_, proxyb_.body_id(), bodyb->body_id());
    dJointSetHingeAxis(hingeb_, 0, 0, 1);
    
    rope_ = dJointCreateSlider(LEVEL->world, 0);
    vec axis = bpos - apos;
    dJointAttach(rope_, proxya_.body_id(), proxyb_.body_id());
    dJointSetSliderAxis(rope_, axis.x, axis.y, 0);
    
    dJointSetSliderParam(rope_, dParamLoStop, 0);
    dJointSetSliderParam(rope_, dParamStopCFM, 0.25);
    dJointSetSliderParam(rope_, dParamStopERP, 0.01);

    ext_ = base_ext_ = axis.norm();
}

Rope::~Rope() {
    dJointDestroy(hingea_);
    dJointDestroy(hingeb_);
    dJointDestroy(rope_);
}

void Rope::draw() {
    vec posa = proxya_.position();
    vec posb = proxyb_.position();

    glPushAttrib(GL_LINE_BIT);
        if (selected_) {
            glLineWidth(2.0);
            glColor3d(0,1,0);
        }
        else {
            glColor3d(1,1,1);
        }
        glBegin(GL_LINES);
            glVertex2d(posa.x, posa.y);
            glVertex2d(posb.x, posb.y);
        glEnd();
    glPopAttrib();
}

void Rope::lengthen(num amt) {
    ext_ += amt;
    if (ext_ < 0) {
        ext_ = 0;
    }
    dJointSetSliderParam(rope_, dParamLoStop, base_ext_ - ext_);
}

bool collide(Object* a, Object* b, bool swap) {
    // Poor man's MMD
    if (Spikey* spikey = dynamic_cast<Spikey*>(a)) {
        if (Spikey* spikey2 = dynamic_cast<Spikey*>(b)) {
            spikey->unstick();
            spikey2->unstick();
            return true;
        }
        else if (spikey->state() == Spikey::STICKY) {
            if (dynamic_cast<Wall*>(b)) {
                spikey->stick(0);
            }
        }
        else if (spikey->state() == Spikey::STUCK) {
            return false;
        }
    }
    return true;
}
