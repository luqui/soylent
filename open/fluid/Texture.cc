#include "Texture.h"
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
        Texture* newtex = new Texture(filename, NULL);
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

Texture::Texture(const string& filename, SDL_Surface* incoming)
    : key_(filename), w_(1), h_(1)
{
	SDL_Surface* surf;

	if (incoming != NULL) {
		surf = incoming;
	}
	else {
		if (texture_cache.find(filename) != texture_cache.end()) {
			std::cerr << "Attempt to load already-loaded texture " << filename << "\n";
			exit(1);
		}
	    
		surf = IMG_Load(filename.c_str());
		if (!surf) {
			std::cerr << "Couldn't load " << filename << ": " << IMG_GetError() << "\n";
		}
	}

	GLenum format;
	switch (surf->format->BytesPerPixel) {
		case 1:
			format = GL_LUMINANCE;  // hack: we don't know how to tell between lum and lumalpha
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
			std::cerr << "Don't know how to deal with " << surf->format->BytesPerPixel << " bytes per pixel\n";
			exit(1);
	}

    glPushAttrib(GL_TEXTURE_BIT);
	glGenTextures(1, &texid_);

	SDL_LockSurface(surf);
    if (parity(surf->h) != 1 || parity(surf->w) != 1) {
		h_ = surf->h;
		w_ = surf->w;
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, texid_);
		glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA,
					surf->w, surf->h, 0, format, 
					GL_UNSIGNED_BYTE, surf->pixels);
		glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
	else {    
		glBindTexture(GL_TEXTURE_2D, texid_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
					surf->w, surf->h, 0, format, 
					GL_UNSIGNED_BYTE, surf->pixels);
	}
	SDL_UnlockSurface(surf);

    glPopAttrib();
	
	if (!incoming) {
		SDL_FreeSurface(surf);
	}
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

	if (w_ != 1 || h_ != 1) {
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, texid_);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texid_);
	}

    return TextureBinding(this);
}