#include "Component.h"
#include "ContextMenu.h"
#include <GL/gl.h>
#include <GL/glu.h>

ComponentSlot::ComponentSlot(Component* owner, vec2 offset, EnergyType energy)
    : owner_(owner)
    , destination_(0)
    , offset_(offset)
    , type_(energy)
{ }

ComponentSlot::~ComponentSlot()
{
    detach();
}

void ComponentSlot::step()
{
    if (connected()) {
        { // attract
            vec2 force = (destination()->get_position() - get_position());
            if (force.norm2() > 0.01) {
                force = ~force / force.norm2();
            }
            vec2 pos = get_position();
            dBodyAddForceAtPos(owner()->get_body(), 
                               force.x, force.y, 0, 
                               pos.x, pos.y, 0);
        }
        { // repel
            vec2 force = -(get_position() + destination()->get_position() - 2*destination()->owner()->get_position());
            if (force.norm2() > 0.01) {
                force = ~force / force.norm2();
            }
            vec2 pos = get_position();
            dBodyAddForceAtPos(owner()->get_body(), 
                               force.x, force.y, 0, 
                               pos.x, pos.y, 0);
        }

    }
}

void ComponentSlot::draw() const
{
    if (connected()) {
        vec2 mypos = get_position();
        vec2 opos = destination()->get_position();
        
        glColor3f(0,0.3,0);
        glBegin(GL_LINES);
        glVertex2f(mypos.x, mypos.y);
        glVertex2f(opos.x, opos.y);
        glEnd();
    }

    if (pick_push()) {
        vec2 pos = owner_->get_position();
        
        glPushMatrix();
        glTranslatef(pos.x, pos.y, 0);
        glRotatef(owner_->get_rotation() * 180 / M_PI, 0, 0, 1);
        glTranslatef(offset_.x, offset_.y, 0);
        
        glBegin(GL_QUADS);
        glVertex2f(-0.25, -0.25);
        glVertex2f( 0.25, -0.25);
        glVertex2f( 0.25,  0.25);
        glVertex2f(-0.25,  0.25);
        glEnd();
        glPopMatrix();
    }
    pick_pop();
}

void ComponentSlot::detach()
{
    if (connected()) {
        destination_->on_detach();
        on_detach();
    }
}

vec2 ComponentSlot::get_position() const
{
    return owner_->get_position() + offset_.rotate(owner_->get_rotation());
}

void ComponentSlot::connect(ComponentSlot* a, ComponentSlot* b)
{
    a->detach();
    b->detach();

    a->destination_ = b;
    b->destination_ = a;
}

void ComponentSlot::on_detach() 
{
    destination_ = NULL;
}


void ComponentSlot::on_pick()
{
    vec2 poffset;
    vec2 mypos = get_position();
    if (mypos.x > owner_->get_position().x) {
        poffset = vec2(1,0);
    }
    else {
        poffset = vec2(-1,0);
    }
    OBJECTS.insert(new ContextMenu::Menu(get_position() + poffset));
}

Component::Component()
    : bodyid_(dBodyCreate(WORLD))
    , plane2d_(dJointCreatePlane2D(WORLD, 0))
{
    dJointAttach(plane2d_, bodyid_, 0);
}

Component::~Component()
{
    dBodyDestroy(bodyid_);
    dJointDestroy(plane2d_);
}

void Component::step()
{
    apply_damping(bodyid_, 1,   0.05);
    //                    linear  angular
    
    slots_t mySlots = slots();
    for (slots_t::iterator i = mySlots.begin(); i != mySlots.end(); ++i) {
        (*i)->step();
    }
}

void Component::draw() const
{
    slots_t mySlots = slots();
    
    for (slots_t::iterator i = mySlots.begin(); i != mySlots.end(); ++i) {
        (*i)->draw();
    }
}

Component::slots_t Component::open_slots() const
{
    slots_t mySlots = slots();
    slots_t ret;
    for (slots_t::const_iterator i = mySlots.begin(); i != mySlots.end(); ++i) {
        if (!(*i)->connected()) {
            ret.push_back(*i);
        }
    }
    return ret;
}

void Component::initialize(const ComponentTemplate* temp)
{
    for (ComponentTemplate::slots_t::const_iterator i = temp->slots.begin();
            i != temp->slots.end(); ++i) {
        slots_.push_back(new ComponentSlot(this, i->offset, i->type));
    }
}
