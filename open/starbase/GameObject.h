#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

class GameObject
{
public:
    virtual ~GameObject() { }
    
    virtual void draw() const { }
    virtual void step() { }
};

#endif
