#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "soy/config.h"
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>

using std::string;

class  TextureBinding {  
    friend class Texture;
public:
    DLLEXPORT ~TextureBinding();
private:
    TextureBinding(const Texture* tex) : tex_(tex) { }
    const Texture* tex_;
};

class Texture
{
public:
    DLLEXPORT Texture(const string& filename);
    DLLEXPORT Texture(SDL_Surface* surf);
    DLLEXPORT ~Texture();

    GLuint tex_id() const { return texid_; }
    TextureBinding DLLEXPORT bind_tex() const;
    
private:
    void load_surface(SDL_Surface* surf);
    
    static int textures_alloced;
    
    string key_;
    GLuint texid_;
    bool blend_;
};

Texture DLLEXPORT * load_texture(const string& filename);

#endif
