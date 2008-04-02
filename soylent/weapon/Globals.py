import pygame
from OpenGL.GL import *
from OpenGL.GLU import *
from pygame import *

g_screenWidth = 1024
g_screenHeight = 768
g_worldWidth = 4000
g_worldHeight = 3000
g_friction = 0.92
g_HeroTexture = 0
g_EnemyTexture = 1
g_OrbTexture = 2

def LoadTexture(filename, texture):
    surface = pygame.image.load(filename)
    data = pygame.image.tostring(surface, "RGBA", 1)
    
    glBindTexture(GL_TEXTURE_2D, texture)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, surface.get_width(), surface.get_height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, data );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    
    return surface.get_rect()

def DrawTexture(texture, pos, size):
    glBindTexture(GL_TEXTURE_2D, texture)
    glPushMatrix()
    glTranslatef(pos[0], pos[1], 0)
    glScalef(size[0], size[1], 0)
    glBegin(GL_QUADS)
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0)    # Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0)    # Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0)    # Top Right Of The Texture and Quad
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0)    # Top Left Of The Texture and Quad
    glEnd()
    glPopMatrix()