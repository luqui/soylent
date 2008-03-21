#include "soy/error.h"
#include "soy/Texture.h"
#include "soy/error.h"
#include "SDL_image.h"
#include <iostream>
#include <map>
#include <sstream>

using std::map;

typedef map<string, Texture*> texture_cache_t; 
static texture_cache_t texture_cache;

int Texture::textures_alloced = 0;

Texture DLLEXPORT * load_texture(const string& filename)
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

DLLEXPORT TextureBinding::~TextureBinding()
{
    glPopAttrib();
}

void Texture::load_surface(SDL_Surface* surf)
{
    if (parity(surf->h) != 1 || parity(surf->w) != 1) {
        DIE("Image " + key_ + " does not have height and width powers of 2");
    }

    glPushAttrib(GL_TEXTURE_BIT);

    glGenTextures(1, &texid_);
    glBindTexture(GL_TEXTURE_2D, texid_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    switch (surf->format->BytesPerPixel) {
        case 1:
            format = GL_LUMINANCE;
            blend_ = false;
            break;
        case 3:
            format = GL_RGB;
            blend_ = false;
            break;
        case 4:
            format = GL_RGBA;
            blend_ = true;
            break;
        default:
            DIE("I don't know about strange numbers of bytes per pixel!");
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, (blend_ ? GL_RGBA : GL_RGB),
                 surf->w, surf->h, 0, format, 
                 GL_UNSIGNED_BYTE, surf->pixels);

    if (glGetError() != GL_NO_ERROR) {
        DIE("There was some kind of opengl error");
    }

    glPopAttrib();
}

DLLEXPORT Texture::Texture(const string& filename)
    : key_(filename), texid_(~0)
{
    if (texture_cache.find(filename) != texture_cache.end()) {
        DIE("Attempt to load already-loaded texture " + filename);
    }
    
    SDL_Surface* surf = IMG_Load(filename.c_str());
    surf || DIE("Couldn't load " + filename + ": " + IMG_GetError());

    load_surface(surf);

    SDL_FreeSurface(surf);
}

DLLEXPORT Texture::Texture(SDL_Surface* surf) 
    : key_(""), texid_(~0)
{
    load_surface(surf);
}

DLLEXPORT Texture::~Texture() 
{
    glDeleteTextures(1, &texid_);
    if (key_ != "") {
        texture_cache.erase(key_);
    }
}

TextureBinding DLLEXPORT Texture::bind_tex() const {
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
