#include "drawing.h"
#include <map>

static unsigned int parity(unsigned int in) {
    unsigned int p = 0;
    for (unsigned int i = 0; i < 8*sizeof(in); i++) {
        if (in & (1 << i)) p++;
    }
    return p;
}

Texture::Texture(const string& file) {
    SDL_Surface* img = IMG_Load(file.c_str());
    img || DIE("Couldn't load " + file + ": " + IMG_GetError());
    make_texture(img);
    SDL_FreeSurface(img);
}

Texture::~Texture() {
    glDeleteTextures(1, &texid_);
}

void Texture::make_texture(SDL_Surface* img) {
    if (parity(img->w) != 1 || parity(img->h) != 1) {
        SDL_FreeSurface(img);
        DIE("Image does not have height & width powers of 2");
    }
    
    glPushAttrib(GL_TEXTURE_BIT);

    glGenTextures(1, &texid_);
    glBindTexture(GL_TEXTURE_2D, texid_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 
                 img->w, img->h, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, 
                 img->pixels);

    glPopAttrib();
}

typedef map<string, Texture*> loaded_textures_t;
loaded_textures_t loaded_textures;

Texture* make_texture(const string& file) {
    loaded_textures_t::iterator i = loaded_textures.find(file);
    if (i != loaded_textures.end()) {
        return i->second;
    }
    else {
        return i->second = new Texture(file);
    }
}

void free_texture(const string& file) {
    loaded_textures_t::iterator i = loaded_textures.find(file);
    if (i != loaded_textures.end()) {
        delete i->second;
        loaded_textures.erase(i);
    }      
}

void draw_circle() {
    static int dlist = 0;
    if (dlist) {
        glCallList(dlist);
    }
    else {
        dlist = glGenLists(1);
        glNewList(dlist, GL_COMPILE_AND_EXECUTE);
            num theta = 0;
            glBegin(GL_TRIANGLE_FAN);
                glVertex2d(0,0);
                for (int i = 0; i <= 24; i++, theta += M_PI/12) {
                    glVertex2d(cos(theta), sin(theta));
                }
            glEnd();
        glEndList();
    }
}

void draw_circle(num radius) {
    glPushMatrix();
    glScaled(radius, radius, 0);
    draw_circle();
    glPopMatrix();
}

void draw_box(vec ll, vec ur) {
    glBegin(GL_QUADS);
        glVertex2d(ll.x, ll.y);
        glVertex2d(ur.x, ll.y);
        glVertex2d(ur.x, ur.y);
        glVertex2d(ll.x, ur.y);
    glEnd();
}
