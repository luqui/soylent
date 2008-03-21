#ifndef __PICKABLE_H__
#define __PICKABLE_H__

#include <GL/gl.h>
#include <GL/glu.h>

class Pickable
{
public:
    virtual ~Pickable() { }
    
    bool pick_push() const { 
        GLint rendermode;
        glGetIntegerv(GL_RENDER_MODE, &rendermode);
        glPushName(reinterpret_cast<GLuint>(this));
        if (rendermode == GL_SELECT) {
            return true;
        }
        else {
            return false;
        }
    }

    void pick_pop() const  { glPopName(); }

    virtual void on_pick() = 0;
};

#endif
