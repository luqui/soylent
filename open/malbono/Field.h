#ifndef __FIELD_H__
#define __FIELD_H__

#include "Element.h"
#include "Terrain.h"
#include <GL/gl.h>
#include <iostream>

const int FIELDW = 160;
const int FIELDH = 120;

class Field {
public:
    void diffuse(double diff, double dt) {
        diff *= dt;

        double pressure[FIELDW][FIELDH];

        for (int i = 0; i < FIELDW; i++) {
            for (int j = 0; j < FIELDH; j++) {
                double n = field_[i][j].norm2();
                if (n > 4) {
                    field_[i][j] = 2 * field_[i][j] / sqrt(n);
                    pressure[i][j] = 4;
                }
                else {
                    pressure[i][j] = n;
                }
                pressure[i][j] += terrain_[i][j].potential();
            }
        }

        Element (*oldfield)[FIELDH] = new field_t;
        memcpy(*oldfield, field_, sizeof(field_t));

        for (int i = 1; i < FIELDW-1; i++) {
            for (int j = 1; j < FIELDH-1; j++) {
                double tx1 = Terrain::trans(terrain_[i-1][j], terrain_[i][j]);
                double tx2 = Terrain::trans(terrain_[i+1][j], terrain_[i][j]);
                double ty1 = Terrain::trans(terrain_[i][j-1], terrain_[i][j]);
                double ty2 = Terrain::trans(terrain_[i][j+1], terrain_[i][j]);

                field_[i][j] += diff * tx1 * oldfield[i-1][j];
                field_[i][j] += diff * tx2 * oldfield[i+1][j];
                field_[i][j] += diff * ty1 * oldfield[i][j-1];
                field_[i][j] += diff * ty2 * oldfield[i][j+1];
                field_[i][j] -= diff * (tx1+tx2+ty1+ty2) * oldfield[i][j];

                vel_[i][j] += dt * 
                            ( (pressure[i-1][j] - pressure[i][j]) * tx1 * vec2(1,0)
                            + (pressure[i+1][j] - pressure[i][j]) * tx2 * vec2(-1,0)
                            + (pressure[i][j-1] - pressure[i][j]) * ty1 * vec2(0,1)
                            + (pressure[i][j+1] - pressure[i][j]) * ty2 * vec2(0,-1)
                            ) ;
            }
        }

        delete oldfield;
    }

    void advect(double dt) {
        Element (*oldfield)[FIELDH] = new field_t;
        memcpy(*oldfield, field_, sizeof(field_t));

        for (int i = 1; i < FIELDW-1; i++) {
            for (int j = 1; j < FIELDH-1; j++) {
                const Element& fx1 = oldfield[i-1][j];
                double vx1 = vel_[i-1][j].x;
                const Element& fx2 = oldfield[i+1][j];
                double vx2 = -vel_[i+1][j].x;
                const Element& fy1 = oldfield[i][j-1];
                double vy1 = vel_[i][j-1].y;
                const Element& fy2 = oldfield[i][j+1];
                double vy2 = -vel_[i][j+1].y;

                field_[i][j] = dt * (vx1*fx1 + vx2*fx2 + vy1*fy1 + vy2*fy2)
                             + (1 + dt*(vx1+vx2+vy1+vy2))*oldfield[i][j];
            }
        }

        delete oldfield;
    }

    void draw() {
        draw_terrain();

        const double dx = 32.0/FIELDW, dy = 24.0/FIELDH;
        glBegin(GL_QUADS);
        double x = 0;
        for (int i = 0; i < FIELDW-1; i++) {
            double y = 0;
            for (int j = 0; j < FIELDH-1; j++) {
                field_[i]  [j]  .set_color();     glVertex2f(x   , y);
                field_[i+1][j]  .set_color();     glVertex2f(x+dx, y);
                field_[i+1][j+1].set_color();     glVertex2f(x+dx, y+dy);
                field_[i]  [j+1].set_color();     glVertex2f(x   , y+dy);
                y += dy;
            }
            x += dx;
        }
        glEnd();
    }

    void texdraw() {
        draw_terrain();

        Uint32* pixptr = (Uint32*)surf_->pixels;
        double r,g,b,a;
        const double max = 255.0/256.0;
        for (int j = FIELDH-1; j >= 0; j--) {
            for (int i = 0; i < FIELDW; i++) {
                field_[i][j].color(&r,&g,&b,&a);
                if (r < 0) r = 0; else if (r >= 1) r = max;
                if (g < 0) g = 0; else if (g >= 1) g = max;
                if (b < 0) b = 0; else if (b >= 1) b = max;
                if (a < 0) a = 0; else if (a >= 1) a = max;
                Uint8 ri = Uint8(256*r);
                Uint8 gi = Uint8(256*g);
                Uint8 bi = Uint8(256*b);
                Uint8 ai = Uint8(256*a);
                *pixptr++ = (ri) | (gi << 8) | (bi << 16) | (ai << 24);
            }
            pixptr += surf_->w - FIELDW;
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf_->w, surf_->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf_->pixels);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1,1,1);
        
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); 
            glVertex2f(0,24);
            glTexCoord2f(float(FIELDW)/surf_->w, 0);
            glVertex2f(32,24);
            glTexCoord2f(float(FIELDW)/surf_->w, float(FIELDH)/surf_->h);
            glVertex2f(32,0);
            glTexCoord2f(0, float(FIELDH)/surf_->h);
            glVertex2f(0,0);
        glEnd();
    }

    void randomize() {
        glGenTextures(1,&tex_);
        surf_ = SDL_CreateRGBSurface(SDL_HWSURFACE, 256, 128, 32, 
                                0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

        const int subs = 4;
        for (int si = 0; si < subs; si++) {
            for (int sj = 0; sj < subs; sj++) {
                Element rnd;
                rnd.randomize();
                rnd /= rnd.norm();
                
                for (int i = si*FIELDW/subs; i < (si+1)*FIELDW/subs; i++) {
                    for (int j = sj*FIELDH/subs; j < (sj+1)*FIELDH/subs; j++) {
                        field_[i][j] = rnd;
                    }
                }
            }
        }

        for (int i = 0; i < FIELDW; i++) {
            for (int j = 0; j < FIELDH/4; j++) {
                field_[i][j] = Element();
                terrain_[i][j] = Terrain(Terrain::ROCK);
            }
        }

        /*
        for (int i = 0; i < FIELDW/2; i++) {
            for (int j = 0; j < FIELDH/2; j++) {
                field_[i][j].component[0] = 1;
            }
        }
        for (int i = FIELDW/2; i < FIELDW; i++) {
            for (int j = 0; j < FIELDH/2; j++) {
                field_[i][j].component[1] = 1;
            }
        }
        for (int i = 0; i < FIELDW/2; i++) {
            for (int j = FIELDH/2; j < FIELDH; j++) {
                field_[i][j].component[0] = -1;
            }
        }
        for (int i = FIELDW/2; i < FIELDW; i++) {
            for (int j = FIELDH/2; j < FIELDH; j++) {
                field_[i][j].component[1] = -1;
            }
        }
        */
        
        update_terrain();
    }

    void move(double x, double y, double xt, double yt) {
        int ip = int(xt * FIELDW / 32);
        int jp = int(yt * FIELDW / 32);
        if (ip - 10 < 0 || ip + 10 >= FIELDW || jp - 10 < 0 || jp + 10 >= FIELDH) {
            return;
        }

        vec2 vec(xt-x,yt-y);
        vec /= 10;
        for (int i = ip-10; i <= ip+10; i++) {
            for (int j = jp-10; j <= jp+10; j++) {
                vel_[i][j] += vec;
            }
        }
    }

    void erase(double x, double y, double dt) {
        int ip = int(x * FIELDW / 32);
        int jp = int(y * FIELDW / 32);
        if (ip - 10 < 0 || ip + 10 >= FIELDW || jp - 10 < 0 || jp + 10 >= FIELDH) {
            return;
        }
        for (int i = ip-10; i <= ip+10; i++) {
            for (int j = jp-10; j <= jp+10; j++) {
                field_[i][j] /= 1 + dt;
            }
        }
    }

private:
    void draw_terrain() const {
        for (chunks_t::const_iterator i = chunks_.begin(); i != chunks_.end(); ++i) {
            i->draw(vec2(0,0), vec2(32,24));
        }
    }

    void update_terrain() {
        chunks_ = TerrainChunk::make_chunk_list((Terrain*)terrain_, FIELDW, FIELDH);
        std::cout << "Updated terrain: chunks = " << chunks_.size() << "\n";
    }

    typedef Element field_t[FIELDW][FIELDH];
    field_t field_;
    typedef vec2 vel_t[FIELDW][FIELDH];
    vel_t vel_;
    typedef Terrain terrain_t[FIELDW][FIELDH];
    terrain_t terrain_;
    typedef std::list<TerrainChunk> chunks_t;
    chunks_t chunks_;

    GLuint tex_;
    SDL_Surface* surf_;
};

#endif
