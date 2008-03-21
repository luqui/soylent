#include "Terrain.h"
#include "Texture.h"
#include "Draw.h"

PlainsTerrain::PlainsTerrain(const vec& pos)
    : Terrain(pos),
      tex_(load_texture("media/ground.png"))
{ }

void PlainsTerrain::render() const
{
    TextureBinding b = tex_->bind();
    Draw::texture_square();
}


Tree::Tree(const vec& pos)
    : pos_(pos), treetex_(load_texture("media/tree.png"))
{ }

void Tree::render() const
{
    TextureBinding b = treetex_->bind();
    Draw::texture_square();
}


ForestTerrain::ForestTerrain(const vec& pos, num density)
    : PlainsTerrain(pos),
      density_(density)
{
    num ypos = randrange(0,density_);
    while (ypos < 1) {
        trees_.push_front(new Tree(pos + vec(randrange(0,1), ypos)));
        ypos += randrange(0,density_);
    }
}

void ForestTerrain::draw(Artist* artist, const tvec& ref) const {
    artist->enqueue(ref, this);
    for (trees_t::const_iterator i = trees_.begin(); i != trees_.end(); ++i) {
        (*i)->draw(artist, ref);
    }
}


TerrainArray::TerrainArray(const vec& origin, int w, int h)
    : origin_(origin), w_(w), h_(h), array_(w)
{
    for (int i = 0; i < w; i++) {
        array_[i].resize(h);
        for (int j = 0; j < h; j++) {
            if (i < 2) {
                array_[i][j] = new PlainsTerrain(origin + vec(i,j));
            }
            else {
                num density = num(w - i) / w;
                if (density < 0.15) density = 0.15;
                array_[i][j] = new ForestTerrain(origin + vec(i,j), density);
            }
        }
    }
}

void TerrainArray::draw(Artist* artist, const tvec& ref) {
    for (int i = 0; i < array_.size(); i++) {
        for (int j = 0; j < array_[i].size(); j++) {
            array_[i][j]->draw(artist, ref);
        }
    }
}

void TerrainArray::step() {
    for (int i = 0; i < array_.size(); i++) {
        for (int j = 0; j < array_[i].size(); j++) {
            array_[i][j]->step();
        }
    }
}

Terrain* TerrainArray::lookup(const vec& pos) {
    vec vidx = pos - origin_;
    int idxx = int(vidx.x), idxy = int(vidx.y);
    if (idxx < 0 || idxx >= w_ || idxy < 0 || idxy >= h_) {
        return 0;
    }
    else {
        return array_[idxx][idxy];
    }
}
