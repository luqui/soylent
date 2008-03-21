#ifndef __POWERCOMPONENT_H__
#define __POWERCOMPONENT_H__

#include "Component.h"
#include <soy/Texture.h>

class PowerComponent : public Component
{
public:
    PowerComponent()
        : tex_("res/powercomponent.png")
        , geom_(dCreateBox(SPACE, 1, 1, 1))
    {
        dGeomSetBody(geom_, bodyid_);
    }

    ~PowerComponent()
    {
        for (slots_t::iterator i = slots_.begin(); i != slots_.end(); ++i) {
            delete *i;
        }
        dGeomDestroy(geom_);
    }

    slots_t slots() const {
        return slots_;
    }
    
    void draw() const {
        Component::draw();
        
        glPushMatrix();
        vec2 pos = dbody_get_position(bodyid_);
        glTranslatef(pos.x, pos.y, 0);
        glRotatef(180 / M_PI * dbody_get_rotation(bodyid_), 0, 0, 1);

        {
            TextureBinding b = tex_.bind_tex();
            glColor3f(1,1,1);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);   glVertex2f(-0.5, 0.5);
            glTexCoord2f(0,1);   glVertex2f(-0.5,-0.5);
            glTexCoord2f(1,1);   glVertex2f( 0.5,-0.5);
            glTexCoord2f(1,0);   glVertex2f( 0.5, 0.5);
            glEnd();
        }
        
        glPopMatrix();
    }
private:
    Texture tex_;
    dGeomID geom_;
};


class PowerComponentTemplate : public ComponentTemplate
{
public:
    PowerComponentTemplate()
        : tex_("res/powercomponent.png")
    {
        const double scale = 0.35;
        slots.push_back(SlotDef(vec2( scale, 0), ENERGY_GREEN));
        slots.push_back(SlotDef(vec2(-scale, 0), ENERGY_BLUE));
        slots.push_back(SlotDef(vec2(0,  scale), ENERGY_YELLOW));
        slots.push_back(SlotDef(vec2(0, -scale), ENERGY_RED));
    }

    PowerComponent* create() const {
        PowerComponent* comp = new PowerComponent;
        comp->initialize(this);
        return comp;
    }

    void draw() const {
        TextureBinding b = tex_.bind_tex();
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);   glVertex2f(-0.5, 0.5);
        glTexCoord2f(0,1);   glVertex2f(-0.5,-0.5);
        glTexCoord2f(1,1);   glVertex2f( 0.5,-0.5);
        glTexCoord2f(1,0);   glVertex2f( 0.5, 0.5);
        glEnd();
    }

private:
    Texture tex_;
};


#endif
