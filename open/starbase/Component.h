#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "physics.h"
#include "GameObject.h"
#include "Energy.h"
#include "Pickable.h"
#include <vector>
#include <list>

class Component;

class ComponentSlot : public GameObject, public Pickable
{
public:
    ComponentSlot(Component* owner, vec2 offset, EnergyType energy);

    ~ComponentSlot();

    void step();
    void draw() const;
    
    Component* owner() const { return owner_; }
    ComponentSlot* destination() const { return destination_; }
    bool connected() const { return destination() != 0; }
    void detach();
    vec2 get_position() const;
    static void connect(ComponentSlot* a, ComponentSlot* b);
    EnergyType get_energy_type();

    void on_pick();

private:
    void on_detach();
    
    Component* owner_;
    ComponentSlot* destination_;
    vec2 offset_;
    EnergyType type_;
};


class ComponentTemplate : public GameObject
{
public:
    virtual ~ComponentTemplate() { }
    
    struct SlotDef {
        SlotDef(vec2 offset, EnergyType type)
            : offset(offset), type(type)
        { }
        
        vec2 offset;
        EnergyType type;
    };
    
    typedef std::list<SlotDef> slots_t;
    slots_t slots;
    
    virtual Component* create() const = 0;
};


class Component : public GameObject
{
public:
    typedef std::vector<ComponentSlot*> slots_t;
    
    Component();
    virtual ~Component();

    void step();
    void draw() const;

    virtual slots_t slots() const = 0;

    void set_position(vec2 pos) { dBodySetPosition(bodyid_, pos.x, pos.y, 0); }
    slots_t open_slots() const;
    dBodyID get_body() const { return bodyid_; }
    vec2 get_position() const { return dbody_get_position(bodyid_); }
    double get_rotation() const { return dbody_get_rotation(bodyid_); }
    
    void initialize(const ComponentTemplate* temp);
    
protected:
    slots_t slots_;
    dBodyID bodyid_;
    dJointID plane2d_;
};

#endif
