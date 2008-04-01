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

def LoadTexture(filename, texture):
    surface = pygame.image.load(filename)
    data = pygame.image.tostring(surface, "RGBA", 1)
    
    glBindTexture(GL_TEXTURE_2D, texture)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, surface.get_width(), surface.get_height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, data );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    
    return surface.get_rect()