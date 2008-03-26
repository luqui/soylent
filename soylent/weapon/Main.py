#! /usr/bin/env python

import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *
from Hero import *
from Enemy import *



class GameMain:
    """The Main Game Class - This class handles the main 
    initialization and creating of the Game."""
    
    def __init__(self):
        """Initialize PyGame"""
        pygame.init()
        
        """Set the window Size"""
        self.width = g_screenWidth
        self.height = g_screenHeight
        self.worldWidth = 2000
        self.worldHeight =1500
        self.toggle_full_screen = False
        """Create the Screen"""
        self.screen = pygame.display.set_mode((self.width, self.height))
        self.world = pygame.Surface((self.worldWidth, self.worldHeight))
        
        """Create the background"""
        self.background = pygame.image.load("images/floor1.png")
        self.background = self.background.convert()
        
        """Create a blank fill region"""
        self.blank = pygame.Surface(self.screen.get_size())
        self.blank = self.blank.convert()
        self.blank.fill((0,0,0))
        
        """Create a surface used for a fade effect"""
        self.fade = pygame.Surface(self.world.get_size())
        self.fade = self.fade.convert()
        self.fade.fill((0,0,0))
        self.fade.set_alpha(50)

        """create the Hero and friendly sprite group"""    
        self.hero = Hero(self.world.get_rect().center)
        self.friendly_sprites = pygame.sprite.RenderPlain((self.hero))
        
        """create the enemies"""
        self.enemy_sprites = pygame.sprite.Group()
        
        """create timer user event""" 
        seed()
        pygame.time.set_timer(pygame.USEREVENT, 1000)


    def MainLoop(self):
        while 1:
            self.EventHandler()
            self.hero.Update()
            for e in self.enemy_sprites:
                e.Update()
            self.CheckCollisions()
            self.DrawAll()
            pygame.time.wait(25)
            
    def DrawAll(self):
        self.world.blit(self.fade, (0, 0))
        #self.world.blit(self.background, (0, 0))
        self.screen.blit(self.blank, (0, 0))
        self.enemy_sprites.draw(self.world)
        self.hero.Draw(self.world)
        s_rect = self.screen.get_rect()
        s_rect.center = self.hero.rect.center
        self.screen.blit(self.world, (0, 0), s_rect)
        """draw the gesture last"""
        self.hero.ges.Draw(self.screen)
        pygame.display.flip()
        
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
                self.enemy_sprites.add(Enemy(Rect(randrange(50, self.worldWidth - 50, 1),randrange(50, self.worldHeight- 50, 1),24,24)))
"""end class GameMain"""
            
if __name__ == "__main__":
    MainWindow = GameMain()
    MainWindow.MainLoop()
