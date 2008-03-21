#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "tweak.h"
#include "SDL_image.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>

using std::string;

class TextureBinding {  
    friend class Texture;
public:
    ~TextureBinding();
private:
    TextureBinding(const Texture* tex) : tex_(tex) { }
    const Texture* tex_;
};

class Texture
{
public:
    Texture(const string& filename, SDL_Surface* incoming);
    ~Texture();

	int get_width() const { return w_; }
	int get_height() const { return h_; }
    GLuint tex_id() const { return texid_; }
    TextureBinding bind() const;
    
private:    
	static int textures_alloced;

    string key_;
    GLuint texid_;
    bool blend_;
	int w_,h_;
};

Texture* load_texture(const string& filename);

#endif
