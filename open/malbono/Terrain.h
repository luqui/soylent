#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <list>
#include <soy/vec2.h>

class Terrain
{
public:
    enum Type {
        ROCK,
        AIR,
        NUM_TYPES
    };

    Terrain(Type typ = AIR) : type_(typ) { }
    double potential() const { return pottable[type_]; }
    Type type() const { return type_; }

    static double trans(const Terrain& a, const Terrain& b) {
        return transtable[a.type_][b.type_];
    }

private:
    Type type_;

    static double transtable[NUM_TYPES][NUM_TYPES];
    static double pottable[NUM_TYPES];
};

void load_terrain_textures();

class TerrainChunk {
public:
    static std::list<TerrainChunk> make_chunk_list(Terrain* ts, int width, int height);

    TerrainChunk(Terrain::Type type, int minx, int miny, int maxx, int maxy, int width, int height)
        : type_(type), minx_(minx), maxx_(maxx), miny_(miny), maxy_(maxy)
        , width_(width), height_(height)
    { }

    // The ll and ur are the bounding coordinates for the full field, 
    // of which this chunk represents a subfield (indicated by the 
    // {max,min}{x,y} parameters).
    void draw(vec2 ll, vec2 ur) const;

private:

    Terrain::Type type_;
    int minx_, miny_, maxx_, maxy_;
    int width_, height_;
};


#endif
