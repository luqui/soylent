#! /usr/bin/env python

import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *

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
        self.hero = Hero()
        """Create the Screen"""
        self.screen = pygame.display.set_mode((self.width
                                               , self.height))
  
        """Create the background"""
        self.background = pygame.Surface(self.screen.get_size())
        self.background = self.background.convert()
        self.background.fill((0,0,0))

        seed()
#        pygame.time.set_timer(pygame.USEREVENT, 2000)
#        self.dragpoints = []
        self.leftDrag_Flag = False
        self.ges = Gesture()


    def MainLoop(self):

        while 1:
            self.EventHandler()
            self.hero.Move()
            self.DrawAll()
            pygame.time.wait(25)
            
    def DrawAll(self):
        self.screen.blit(self.background, (0, 0))
        self.hero.Draw(self.screen)
#        self.surface.blit(image, rect)  
        
        
        pygame.display.flip()
        
    def EventHandler(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT: 
                    sys.exit()
            elif (event.type == pygame.MOUSEBUTTONDOWN) or (event.type == pygame.MOUSEBUTTONUP) or (event.type == pygame.MOUSEMOTION):
                self.hero.MouseEvent(event)
            elif event.type == pygame.KEYDOWN:
                #self.hero.Move()
                #self.hero.Impulse(event)
                if event.key == K_F1:
                    if toggle_full_screen:
                        pygame.display.set_mode((self.width, self.height),pygame.FULLSCREEN)
                    else:
                        pygame.display.set_mode((self.width, self.height))
                    toggle_full_screen = not toggle_full_screen
                elif event.key == K_ESCAPE:
                    sys.exit()
                    
#                    elif event.key == K_SPACE:
                    #self.snake.jump()
               # elif event.key == K_p:
##                elif event.type == pygame.USEREVENT:
##                    self.enemy_sprites.add(Enemy(Rect(randrange(50, self.width - 50, 1),
##                                                      randrange(50, self.height- 50, 1), 32, 32)))


class Hero(pygame.sprite.Sprite):
    def __init__(self):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load("ship1.png")
        self.modules = []
        self.damage = 1
        self.agility = 1
        self.velocity = (0,0)
        self.acceleration = (0,0)
        self.position = (5000, 5000)
        self.shield = 100
        self.hp = 1000
        self.rect = self.image.get_rect()
        self.impulse = 1
        self.x_mo = 0
        self.y_mo = 0
        self.ges = Gesture()
        self.payloads = []
        
    def Draw(self, surface):
        surface.blit(self.image, self.rect)
        self.ges.Draw(surface)
        for p in self.payloads:
            p.Draw(surface)
            
    def Move(self):
        self.ges.Update()      
        """Move your self in one of the 8 directions according to key"""
        xMove = 0;
        yMove = 0;
        
        if pygame.key.get_pressed()[K_a]:
            if pygame.key.get_pressed()[K_w] or pygame.key.get_pressed()[K_s]:
                xMove = -self.impulse / sqrt(2) 
            else:
                xMove = -self.impulse
        if pygame.key.get_pressed()[K_d]:
            if pygame.key.get_pressed()[K_w] or pygame.key.get_pressed()[K_s]:
                xMove = self.impulse / sqrt(2) 
            else:    
                xMove = self.impulse
        if pygame.key.get_pressed()[K_w]:
            if pygame.key.get_pressed()[K_a] or pygame.key.get_pressed()[K_d]:
                yMove = -self.impulse / sqrt(2) 
            else:
                yMove = -self.impulse
        if pygame.key.get_pressed()[K_s]:
            if pygame.key.get_pressed()[K_a] or pygame.key.get_pressed()[K_d]:
                yMove = self.impulse / sqrt(2) 
            else:
                yMove = self.impulse


        if sqrt(self.x_mo**2 + self.y_mo**2) < 8:
            self.x_mo += xMove 
            self.y_mo += yMove
        
        """friction"""
        self.x_mo *= 0.9
        self.y_mo *= 0.9
            
        self.rect.move_ip(self.x_mo, self.y_mo)
        
        for i in self.payloads:
            i.Update()
            if i.isDead():
                self.payloads.remove(i)
        
    def MouseEvent(self, event):
        self.ges.MouseEvent(event)
        
        if event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1:
                #self.payloads = []
                for i in range(len(self.ges.orblist)):
                    x = float(self.ges.orblist[i][0]) - self.rect.center[0]
                    y = float(self.ges.orblist[i][1]) - self.rect.center[1]
                    mag = sqrt(x*x + y*y)
                    self.payloads.append(Payload(self.rect.center, (x/mag, y/mag), (self.ges.orblist[i][0], self.ges.orblist[i][1])))
                    #self.payloads.append(Payload(self.rect.center, 1, (x/mag, y/mag) ))
                self.ges.orblist = []
                self.ges.power = 2
            
class Drag:
    def __init__(self):
        self.pos = []
        self.time = []
        self.length = []

class Payload(pygame.sprite.Sprite):
    def __init__(self, position, direction, hero_pos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load("orb1.png")
        self.rect = self.image.get_rect()
        self.position = position
        self.initial_speed = ((sqrt((hero_pos[0] - position[0])**2 + (hero_pos[1] - position[1])**2)+1) / 80)+4
        self.mass = 1 / self.initial_speed
        self.velocity = map(lambda x: x * self.initial_speed, direction)
        self.rect.center = position
        self.life = 100.0
        self.lifespan = 1500
        self.age = 0
        self.timeborn = pygame.time.get_ticks()

    def Draw(self, surface):
        surface.blit(self.image, self.rect)
        
    def Update(self):
        self.position = (self.position[0] + self.velocity[0], self.position[1] + self.velocity[1])
        self.rect.center = self.position
        self.age = (pygame.time.get_ticks() - self.timeborn)
    
    def isDead(self):
        return self.age > self.lifespan
                       
        
class Gesture:
    def __init__(self):
        self.dragpoints = Drag()
        self.power = 2
        self.starttime = pygame.time.get_ticks()
        self.timesum = 0
        self.leftDrag_Flag = False
        self.startpos = (0,0)
        self.distsum = 0
        self.prevsample = 0
        self.currentlength = 0
        self.orblist = []
        
    def Draw(self, screen):
        if len(self.dragpoints.pos) > 0:
            """Draw evenly spaced circles""" 
            for pos in self.orblist:
                pygame.draw.circle(screen, (100,100,100), pos, 10, 3)        

            """draw red line"""
            if len(self.dragpoints.pos) > 1:
                listpoints = []
                pygame.draw.lines(screen, (0,100,0), False, self.dragpoints.pos, 1)

    def MouseEvent(self, event):
        """handle mouse input"""
        (x1,y1) = pygame.mouse.get_pos()
        ticks = pygame.time.get_ticks()
        
        if event.type == pygame.MOUSEBUTTONDOWN:
            self.startpos = pygame.mouse.get_pos()
            if event.button == 1:
                self.leftDrag_Flag = True
                self.starttime = ticks
                self.prevsample = self.starttime
                self.dragpoints.pos.append(self.startpos)
                self.dragpoints.length.append(0)
                self.orblist = [self.startpos]
                self.orblist.append(self.startpos)
                self.dragpoints.time.append(ticks)
                self.prevsample = ticks      
        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1:
                self.leftDrag_Flag = False
                self.dragpoints = Drag()
                self.currentlength = 0
        elif event.type == pygame.MOUSEMOTION:
            if self.leftDrag_Flag:    
                if ticks - self.prevsample > 10:
                    self.dragpoints.pos.append((x1,y1))
                    self.dragpoints.time.append(ticks)
                    self.prevsample = ticks                               
                    if len(self.dragpoints.pos) > 1:
                        (x2, y2) = self.dragpoints.pos[-2]
                        xd = x1 - x2
                        yd = y1 - y2
                        self.currentlength += sqrt(xd*xd + yd*yd)
                        self.dragpoints.length.append(self.currentlength)
                        
    def Update(self):
        (x1,y1) = pygame.mouse.get_pos()
        ticks = pygame.time.get_ticks()
        if self.leftDrag_Flag:
            if (ticks - self.starttime) / 1000 > self.power - 2:
                self.power += 1
            
            segment_length = self.currentlength / (self.power - 1)
            self.orblist[-1] = (x1, y1)
            c = 1
            for i in range(1, len(self.dragpoints.length)):
                while self.dragpoints.length[i] >= (segment_length * c):
                    diff = self.dragpoints.length[i] - segment_length * c
                    vect = (float(self.dragpoints.pos[i - 1][0] - self.dragpoints.pos[i][0]),
                            (self.dragpoints.pos[i - 1][1] - self.dragpoints.pos[i][1]))
                    mag = sqrt(vect[0]*vect[0] + vect[1]*vect[1])
                    if mag != 0:
                        vect = (vect[0]/mag, vect[1]/mag)
                    if c >= len(self.orblist):
                        self.orblist.append((x1, y1))                    
                    self.orblist[c] = (self.dragpoints.pos[i][0] + diff * vect[0], self.dragpoints.pos[i][1] + diff * vect[1])
                    c+=1
            
if __name__ == "__main__":
    MainWindow = GameMain()
    MainWindow.MainLoop()
