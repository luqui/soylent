#! /usr/bin/env python

import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *
from Hero import *
from Enemy import *

toggle_full_screen = False

class GameMain:
    """The Main Game Class - This class handles the main 
    initialization and creating of the Game."""
    
    def __init__(self, width=1024,height=768):
        """Initialize PyGame"""
        pygame.init()
        
        """Set the window Size"""
        self.width = width
        self.height = height
        
        """Create the Screen"""
        self.screen = pygame.display.set_mode((self.width, self.height))
        
        """Create the background"""
        self.background = pygame.Surface(self.screen.get_size())
        self.background = self.background.convert()
        self.background.fill((0,0,0))


        """create the Hero"""    
        self.hero = Hero()
        self.hero_sprites = pygame.sprite.RenderPlain((self.hero))
        
        """create the enemies"""
        self.enemies = []
        self.enemy_sprites = pygame.sprite.Group()
        
        seed()
        pygame.time.set_timer(pygame.USEREVENT, 2000)
        
        self.leftDrag_Flag = False
        self.ges = Gesture()


    def MainLoop(self):

        while 1:
            self.EventHandler()
            self.hero.Update()
            self.DrawAll()
            pygame.time.wait(25)
            
    def DrawAll(self):
        self.screen.blit(self.background, (0, 0))
        self.hero.Draw(self.screen)   
        self.enemy_sprites.draw(self.screen)
        #for e in self.enemies:
            #e.Draw(self.screen)

        pygame.display.flip()
        
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
                    if toggle_full_screen:
                        pygame.display.set_mode((self.width, self.height),pygame.FULLSCREEN)
                    else:
                        pygame.display.set_mode((self.width, self.height))
                    toggle_full_screen = not toggle_full_screen
                elif event.key == K_ESCAPE:
                    sys.exit()
                elif event.key == K_SPACE:
                    self.hero.Jump()
                    #elif event.key == K_p: #pause
                    
                    """timer event"""
            if event.type == pygame.USEREVENT:
                self.enemies.append(Enemy(Rect(randrange(50, self.width - 50, 1),randrange(50, self.height- 50, 1),24,24)))
                self.enemy_sprites.add(Enemy(Rect(randrange(50, self.width - 50, 1),randrange(50, self.height- 50, 1),24,24)))

            
if __name__ == "__main__":
    MainWindow = GameMain()
    MainWindow.MainLoop()
