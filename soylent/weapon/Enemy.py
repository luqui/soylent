import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *
from Globals import *

class Enemy(pygame.sprite.Sprite):
    
    def __init__(self, rect=None):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load("images/enemy1.png")
        self.rect = self.image.get_rect()
        self.rect.center = rect.center
        self.hp = 100
        self.minions = []
        self.mass = 100
        self.velocity = (0,0)
        
    def Impact(self, mass, velocity):
        self.velocity = self.velocity[0] + (mass/self.mass) * velocity[0], self.velocity[1] + (mass/self.mass) * velocity[1]
    
    def Collide (self, target):
        if not target.isJumping: self.kill()
        
    def Update(self):
        self.rect.move_ip(self.velocity)
        self.velocity = (g_friction*self.velocity[0], g_friction*self.velocity[1])
"""end class Enemy"""