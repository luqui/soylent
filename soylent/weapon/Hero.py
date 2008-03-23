import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *
from Globals import *

class Hero(pygame.sprite.Sprite):
    def __init__(self, position):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load("images/ship1.png")
        self.jumpimage = pygame.image.load("images/ship1_jump.png")
        self.rect = self.image.get_rect()
        self.rect.move_ip(position)
        self.modules = []
        self.damage = 1
        self.agility = 1
        self.velocity = (0,0)
        self.acceleration = (0,0)
        self.shield = 100
        self.hp = 1000        
        self.impulse = 1
        self.x_mo = 0
        self.y_mo = 0
        self.ges = Gesture()
        self.payloads = []
        self.airtime = 280 ##duration of jump
        self.isJumping = False        
        
    def Draw(self, surface):
        if self.isJumping: surface.blit(self.jumpimage, self.rect.move(-16,-16))
        else: surface.blit(self.image, self.rect)
        for p in self.payloads:
            p.Draw(surface)
        
    def GesDraw(self, surface):
        self.ges.Draw(surface)
            
    def Update(self):
        if self.isJumping and pygame.time.get_ticks() - self.startjump >= self.airtime:
            self.isJumping = False 
        self.ges.Update()
        self.Move()

        
    def Move(self):
        
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
        self.x_mo *= 0.91
        self.y_mo *= 0.91
            
        self.rect.move_ip(self.x_mo, self.y_mo)
        
        for i in self.payloads:
            i.Update()
            if i.isDead():
                self.payloads.remove(i)
                
    def Jump(self):
        self.startjump = pygame.time.get_ticks()
        self.isJumping = True
        
    def MouseEvent(self, event):
        self.ges.MouseEvent(event)
        
        if event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1:
                for i in range(len(self.ges.orblist)):
                    x = float(self.ges.orblist[i][0]) - g_screenWidth/2 
                    y = float(self.ges.orblist[i][1]) - g_screenHeight/2
                    mag = sqrt(x*x + y*y)
                    self.payloads.append(Payload(self.rect.center, (x/mag, y/mag), (self.ges.orblist[i][0], self.ges.orblist[i][1])))
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
        self.image = pygame.image.load("images/orb1.png")
        self.rect = self.image.get_rect()
        self.position = position
        self.initial_speed = ((sqrt((hero_pos[0] - position[0])**2 + (hero_pos[1] - position[1])**2)+1) / 300)*10+5
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
        self.chargetime = 350
        
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
            self.startpos = (x1, y1)
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
            if (ticks - self.starttime) / self.chargetime > self.power - 2:
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
