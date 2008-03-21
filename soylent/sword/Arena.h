#ifndef __ARENA_H__
#define __ARENA_H__

#include "physics.h"
#include <GL/gl.h>
#include <GL/glu.h>

class Arena : public Geom
{
public:
    Arena() 
        : Geom(create_plane_geom(vec3(0,0,0), vec3(0,1,0)))
    { }

    void draw() const {
        // Yeah, I know, this seriously needs a refactor
        // Why are we drawing a grid instead of just five squares, you ask?
        // Is so that the lighting looks smooth; i.e. so the whole floor
        // doesn't reflect the same amount of light despite the different angles.
        glColor3f(0.4, 0.2, 0.1);
        glBegin(GL_QUADS);
            glNormal3f(  0, 1,   0);
            for (int i = -16; i < 16; i++) {
                for (int j = -16; j < 16; j++) {
                    glVertex3f(i,   0, j);
                    glVertex3f(i+1, 0, j);
                    glVertex3f(i+1, 0, j+1);
                    glVertex3f(i,   0, j+1);
                }
            }

            glNormal3f(  0,  0, 1);
            for (int i = -16; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    glVertex3f(i,   j,   -16);
                    glVertex3f(i,   j+1, -16);
                    glVertex3f(i+1, j+1, -16);
                    glVertex3f(i+1, j,   -16);
                }
            }

            glNormal3f(  0,  0, -1);
            for (int i = -16; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    glVertex3f(i,   j,   16);
                    glVertex3f(i,   j+1, 16);
                    glVertex3f(i+1, j+1, 16);
                    glVertex3f(i+1, j,   16);
                }
            }
            
            glNormal3f(  1,  0, 0);
            for (int i = -16; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    glVertex3f(-16, j,   i);
                    glVertex3f(-16, j+1, i);
                    glVertex3f(-16, j+1, i+1);
                    glVertex3f(-16, j,   i+1);
                }
            }
            
            glNormal3f( -1,  0, 0);
            for (int i = -16; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    glVertex3f(16, j,   i);
                    glVertex3f(16, j+1, i);
                    glVertex3f(16, j+1, i+1);
                    glVertex3f(16, j,   i+1);
                }
            }
        glEnd();
    }
};

#endif
