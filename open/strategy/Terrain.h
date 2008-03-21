#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "config.h"
#include "Widget.h"
#include "vec.h"
#include "Texture.h"
#include <vector>
#include <list>

using std::vector;
using std::list;

class Terrain : public Widget
{
public:
    Terrain(const vec& pos) : pos_(pos) { }
    virtual ~Terrain() { }

    virtual num gheight(const tvec&) const { return ZG_GROUND; }
    virtual num lheight(const tvec&) const { return ZL_GROUND; }
    virtual vec pos(const tvec&) const { return pos_; }

protected:
    vec pos_;
};

class PlainsTerrain : public Terrain
{
public:
    PlainsTerrain(const vec& pos);
    virtual ~PlainsTerrain() { }

    virtual void render() const;
private:
    Texture* tex_;
};

class Tree : public Widget {
public:
    Tree(const vec& pos);
    virtual ~Tree() { }

    virtual num gheight(const tvec&) const { return ZG_TREES; }
    virtual num lheight(const tvec&) const { return ZL_TREES; }
    virtual vec pos(const tvec&) const { return pos_; }
    virtual void render() const;
    
private:
    vec pos_;
    Texture* treetex_;
};

class ForestTerrain : public PlainsTerrain
{
public:
    ForestTerrain(const vec& pos, num density);
    virtual ~ForestTerrain() { }
    
    virtual void draw(Artist* artist, const tvec& ref) const;
private:
    num density_;
    typedef list< Tree*, gc_allocator<Tree*> > trees_t;
    trees_t trees_;
};

class TerrainArray : public gc
{
public:
    TerrainArray(const vec& origin, int w, int h);
    virtual ~TerrainArray() { }

    virtual void draw(Artist* artist, const tvec& ref);
    virtual void step();

    Terrain* lookup(const vec& pos);

private:
    vec origin_;
        
    typedef vector< Terrain*,    gc_allocator<Terrain*> >    array_row_t;
    typedef vector< array_row_t, gc_allocator<array_row_t> > array_t;

    int w_, h_;
    array_t array_;
};

#endif
