#! /usr/bin/env python

import os, sys
#import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *
from Hero import *
from Enemy import *
from OpenGL.GL import *
from OpenGL.GLU import *

rtri = rquad = 0.0

class GameMain:
    """The Main Game Class - This class handles the main 
    initialization and creating of the Game."""
    
    def __init__(self):
        """Set the window Size"""
        self.width = g_screenWidth
        self.height = g_screenHeight
        self.worldWidth = g_worldWidth
        self.worldHeight = g_worldHeight
        self.worldRect = pygame.Rect((0, 0), (self.worldWidth, self.worldHeight))
        self.toggle_full_screen = False
        self.zoom = -1500
        
        """Initialize PyGame"""
        video_flags = OPENGL|DOUBLEBUF
    
        pygame.init()
        pygame.display.set_mode((self.width,self.height), video_flags)

        self.resize((self.width, self.height))
        self.init()
        
        """create the hero and friends"""
        self.hero = Hero(self.worldRect.center)
        self.friendly_sprites = pygame.sprite.RenderPlain((self.hero))
        
        """create the enemies"""
        self.enemy_sprites = pygame.sprite.Group()
        
        """create timer user event""" 
        seed()
        pygame.time.set_timer(pygame.USEREVENT, 1000)

    def resize(self, (width, height)):
        if height==0:
            height=1
        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45, 1.0*width/height, 0.1, 2000.0)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
    
    def init(self):
        glEnable(GL_TEXTURE_2D)
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glShadeModel(GL_SMOOTH)
        glClearColor(0.0, 0.0, 0.0, 0.0)
        glClearDepth(1.0)
        glEnable(GL_DEPTH_TEST)
        glDepthFunc(GL_LEQUAL)
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    def MainLoop(self):
        frames = 0
        ticks = pygame.time.get_ticks()
        while 1:
            self.EventHandler()
            self.hero.Update()
            for e in self.enemy_sprites:
                e.Update()
            self.CheckCollisions()
            self.DrawAll()
            pygame.time.wait(10)
            frames = frames + 1
            if pygame.time.get_ticks() - ticks > 1000:
                print "fps:  %d" % ((frames*1000)/(pygame.time.get_ticks()-ticks))
                frames = 0
                ticks = pygame.time.get_ticks()
            
    def DrawAll(self):
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
    
        glTranslatef(-self.hero.rect.centerx, -self.hero.rect.centery, self.zoom)
        for enemy in self.enemy_sprites:
            enemy.Draw()
        self.hero.Draw()
        #self.hero.ges.Draw
        
        pygame.display.flip()

#        self.world.blit(self.background, (0, 0))
#        self.enemy_sprites.draw(self.world)
#        self.hero.Draw(self.world)
#        s_rect = self.screen.get_rect()
#        s_rect.center = self.hero.rect.center
#        self.screen.blit(self.world, (0, 0), s_rect)
#        """draw the gesture last"""
#        self.hero.ges.Draw(self.screen)
#        pygame.display.flip()
        
    def CheckCollisions(self):
        """friendlys colliding with enemies"""
        self.hero.Collide(self.enemy_sprites)

    def EventHandler(self):
        for event in pygame.event.get():

            """close button"""
            if event.type == pygame.QUIT: 
                sys.exit()
                
                """mouse event"""
            elif (event.type == pygame.MOUSEBUTTONDOWN) or (event.type == pygame.MOUSEBUTTONUP) or (event.type == pygame.MOUSEMOTION):
                self.hero.MouseEvent(event)
                if event.type == pygame.MOUSEBUTTONDOWN:
                    """zoom in and out on mouse wheel"""
                    if event.button == 4: self.zoom += 100
                    if event.button == 5: self.zoom -= 100
                    if self.zoom < -2000:
                        self.zoom = -2000
                """keyboard event"""
            elif event.type == pygame.KEYDOWN:
                if event.key == K_F1:
                    if self.toggle_full_screen:
                        pygame.display.set_mode((g_screenWidth, g_screenHeight),pygame.FULLSCREEN)
                    else:
                        pygame.display.set_mode((g_screenWidth, g_screenHeight))
                    self.toggle_full_screen = not self.toggle_full_screen
                elif event.key == K_ESCAPE:
                    sys.exit()
                elif event.key == K_SPACE:
                    self.hero.Jump()
                    #elif event.key == K_p: #pause
                    
                    """timer event"""
            if event.type == pygame.USEREVENT:
                self.enemy_sprites.add(Enemy(Rect((randrange(50, self.worldWidth - 50, 1), randrange(50, self.worldHeight - 50, 1)) ,(24,24))))
"""end class GameMain"""
            
if __name__ == "__main__":
    MainWindow = GameMain()
    MainWindow.MainLoop()
