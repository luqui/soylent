#ifndef __CONTEXTMENU_H__
#define __CONTEXTMENU_H__

#include "game.h"
#include "GameObject.h"
#include "Pickable.h"
#include "Component.h"
#include <soy/vec2.h>
#include <memory>
#include <list>

namespace ContextMenu
{

class Menu;

class TemplateItem : public GameObject, public Pickable
{
public:
    TemplateItem(Menu* owner, const ComponentTemplate* templ, vec2 offset) 
        : owner_(owner), templ_(templ), offset_(offset) 
    { }

    void draw() const {
        glPushMatrix();
        glTranslatef(offset_.x, offset_.y, 0);

        pick_push();
        glColor4f(0.6, 0.8, 0.6, 0.3);
        glBegin(GL_QUADS);
        glVertex2f(-0.5, 0.5);
        glVertex2f(-0.5,-0.5);
        glVertex2f( 0.5,-0.5);
        glVertex2f( 0.5, 0.5);
        glEnd();
        
        templ_->draw();
        pick_pop();

        glPopMatrix();
    }

    void on_pick();

private:
    Menu* owner_;
    const ComponentTemplate* templ_;
    vec2 offset_;
};

class Menu : public GameObject
{
public:
    Menu(vec2 pos) : pos_(pos) {
        int width = middlefactor(TEMPLATES.size());

        int x = 0, y = 0;
        for (templates_t::const_iterator i = TEMPLATES.begin();
                i != TEMPLATES.end(); ++i) {
            items_.push_back(new TemplateItem(this, *i, pos_ + vec2(x, -y)));

            x++;
            if (x >= width) {
                x = 0;
                y++;
            }
        }
    }

    ~Menu() {
        for (std::list<TemplateItem*>::iterator i = items_.begin();
                i != items_.end(); ++i) {
            delete *i;
        }
    }

    void draw() const {
        for (std::list<TemplateItem*>::const_iterator i = items_.begin();
                i != items_.end(); ++i) {
            (*i)->draw();
        }
    }
    
private:
    vec2 pos_;
    std::list<TemplateItem*> items_;

    static int middlefactor(int num) {
        // finds the greatest factor of num <= sqrt(num)
        for (int i = int(sqrt(num)); i >= 1; i++) {
            if (num % i == 0) return i;
        }
    }
};

inline void TemplateItem::on_pick()
{
    Component* comp = templ_->create();
    comp->set_position(offset_);
    OBJECTS.insert(comp);
    OBJECTS.erase(owner_);
    delete owner_;
}

}

#endif
