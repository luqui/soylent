#ifndef __SOYLIB_FONT_H__
#define __SOYLIB_FONT_H__

#include "soy/vec2.h"
#include "soy/vec3.h"
#include "soy/Texture.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL_ttf.h>
#include <string>

class Font {
public:
    virtual ~Font() { }

    virtual void draw_string(vec3 where, double height, std::string text) const = 0;
};


class GlutFont : public Font {
public:
    GlutFont(void* font = GLUT_BITMAP_9_BY_15) 
        : font_(font)
    { }

    void draw_string(vec3 where, double height, std::string text) const {
        glRasterPos3d(where.x, where.y, where.z);

        for (size_t i = 0; i < text.size(); i++) {
            glutBitmapCharacter(font_, text[i]);
        }
    }

private:
    void* font_;
};


class TTFont : public Font {
public:
    TTFont(std::string filename, int res) {
        init();
        font_ = TTF_OpenFont(filename.c_str(), res);
    }

    ~TTFont() {
        TTF_CloseFont(font_);
    }

    void draw_string(vec3 where, double height, std::string text) const {
        SDL_Color color = {255,255,255};
        SDL_Surface* surf;
        surf = TTF_RenderText_Solid(font_, text.c_str(), color);
        if (!surf) {
            DIE("TTF Render error: " + TTF_GetError());
        }

        // blit the texture onto a new texture of size power of 2
        SDL_Surface* surfPow2 = SDL_CreateRGBSurface(
                SDL_SRCALPHA, round_up_pow2(surf->w), round_up_pow2(surf->h),
                32, 0xff000000, 0x0000ff00,
                    0x00ff0000, 0x000000ff);

        SDL_BlitSurface(surf, NULL, surfPow2, NULL);

        // compute the upper texcoords for our expansion
        float texcoordx = float(surf->w) / surfPow2->w;
        float texcoordy = float(surf->h) / surfPow2->h;
        float width = height * surf->w / surf->h;

        // we're done with the original
        SDL_FreeSurface(surf);

        Texture tex(surfPow2);

        TextureBinding binding = tex.bind_tex();

        glBegin(GL_QUADS);
            glTexCoord2f(0, texcoordy);
            glVertex3f(where.x, where.y, where.z);
            glTexCoord2f(texcoordx, texcoordy);
            glVertex3f(where.x + width, where.y, where.z);
            glTexCoord2f(texcoordx, 0);
            glVertex3f(where.x + width, where.y + height, where.z);
            glTexCoord2f(0, 0);
            glVertex3f(where.x, where.y + height, where.z);
        glEnd();
        
        SDL_FreeSurface(surfPow2);
    }

private:
    int round_up_pow2(int in) const {
        int start = 1;
        while (start < in) {
            start <<= 1;
        }
        return start;
    }
    
    void init() {
        if (!TTF_WasInit()) {
            if (TTF_Init() == -1) {
                DIE("TTF Initialization error: " + TTF_GetError());
            }
        }
    }

    TTF_Font* font_;
};

#endif
