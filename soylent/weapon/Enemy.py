import os, sys
import pygame
from math import sqrt, sin, cos, pi
from random import *
from pygame import *

class Enemy(pygame.sprite.Sprite):
    
    def __init__(self, rect=None):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load("images/enemy1.png")
        self.rect = self.image.get_rect()
        self.rect.center = rect.center
        self.hp = 100
        self.minions = []
        
    #def Draw(self, screen):
        #screen.blit(self.image, self.rect)
    def Collide (self, target):
        print "Collsion!"
        
    def Update(self):
        print "yay"
