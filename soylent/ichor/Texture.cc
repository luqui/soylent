#include "Texture.h"
#include "SDL_image.h"
#include <map>
#include <sstream>
#include <iostream>

using std::map;

typedef map<string, Texture*> texture_cache_t; 
static texture_cache_t texture_cache;

int Texture::textures_alloced = 0;

Texture* load_texture(const string& filename)
{
    texture_cache_t::iterator i = texture_cache.find(filename);
    if (i == texture_cache.end()) {
        Texture* newtex = new Texture(filename);
        return (texture_cache[filename] = newtex);
    }
    else {
        return i->second;
    }
}

static unsigned int parity(unsigned int in) {
    unsigned int p = 0;
    for (unsigned int i = 0; i < 8*sizeof(in); i++) {
        if (in & (1 << i)) p++;
    }
    return p;
}

TextureBinding::~TextureBinding()
{
    glPopAttrib();
}

Texture::Texture(const string& filename)
    : key_(filename)
{
    if (texture_cache.find(filename) != texture_cache.end()) {
		std::cerr << "Attempt to load already-loaded texture " << filename << "\n";
		exit(1);
    }
    
    SDL_Surface* surf = IMG_Load(filename.c_str());
    if (!surf) {
		std::cerr << "Couldn't load " << filename << ": " << IMG_GetError() << "\n";
	}

    if (parity(surf->h) != 1 || parity(surf->w) != 1) {
		std::cerr << "Image " << filename << " does not have height and width powers of 2\n";
		exit(1);
    }

    blend_ = (surf->format->BytesPerPixel == 4);

    glPushAttrib(GL_TEXTURE_BIT);

    glGenTextures(1, &texid_);
    glBindTexture(GL_TEXTURE_2D, texid_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, surf->format->BytesPerPixel,
                 surf->w, surf->h, 0, (blend_ ? GL_RGBA : GL_RGB), 
                 GL_UNSIGNED_BYTE, surf->pixels);

    glPopAttrib();
	
	SDL_FreeSurface(surf);
}

Texture::~Texture() 
{
    glDeleteTextures(1, &texid_);
    texture_cache.erase(key_);
}

TextureBinding Texture::bind() const {
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
    if (blend_) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
    }
    else {
        glDisable(GL_BLEND);
    }
    
    glBindTexture(GL_TEXTURE_2D, texid_);
    return TextureBinding(this);
}
