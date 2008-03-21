#ifndef __GAME_H__
#define __GAME_H_

#include <set>

class GameObject;
class ComponentTemplate;

typedef std::set<GameObject*> objects_t;
extern objects_t OBJECTS;

typedef std::set<ComponentTemplate*> templates_t;
extern templates_t TEMPLATES;

#endif
