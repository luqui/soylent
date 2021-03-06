import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *
from Globals import *

class Enemy(pygame.sprite.Sprite):
    
    def __init__(self, rect=None):
        pygame.sprite.Sprite.__init__(self)
        self.texture = g_EnemyTexture
        self.rect = LoadTexture("images/enemy1.png", self.texture)
        self.rect.center = rect.center
        self.hp = 5000
        self.armor = 1
        self.minions = []
        self.mass = 100.0
        self.velocity = (0,0)
        self.damage = 10
        
    def Draw(self):
        DrawTexture(self.texture, self.rect.center, self.rect.size)
        
    def Impact(self, mass, velocity, damage):
        self.hp -= mass * sqrt((self.velocity[0] - velocity[0])**2 + (self.velocity[1] - velocity[1])**2)
        self.velocity = self.velocity[0] + (mass/self.mass) * velocity[0], self.velocity[1] + (mass/self.mass) * velocity[1]
        #self.hp -= (damage / self.armor)
    
    def HitHero(self, hero):
        hero.TakeDamage(self.damage)
        
    def Collide (self, target):
        if not target.isJumping: self.kill()
        
    def Update(self):
        self.rect.move_ip(self.velocity)
        self.velocity = (g_friction*self.velocity[0], g_friction*self.velocity[1])
        if self.hp <= 0: self.kill()
"""end class Enemy"""