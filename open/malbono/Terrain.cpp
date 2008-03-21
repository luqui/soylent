#include "Terrain.h"
#include <GL/gl.h>
#include <SDL_image.h>
#include <iostream>

double Terrain::transtable[Terrain::NUM_TYPES][Terrain::NUM_TYPES] =
    // ROCK  AIR
    { { 1.0, 1.0 }  // ROCK
    , { 1.0, 1.0 }  // AIR
    };

double Terrain::pottable[Terrain::NUM_TYPES] =
    { 1.0  // ROCK
    , 0.0  // AIR
    };

static void make_chunk(Terrain* ts, int width, int height, 
                       bool* mark, std::list<TerrainChunk>* ret, 
                       Terrain::Type type, int i, int j) {
    if (mark[i*height+j]) return;

    int maxii = 0, maxjj = 0, maxarea = 0;
    int lubjj = height;
    for (int ii = i; ii < width; ii++) {
        if (mark[ii*height+j] || ts[ii*height+j].type() != type) break;

        for (int jj = j; jj < lubjj; jj++) {
            if (mark[ii*height+jj] || ts[ii*height+jj].type() != type) {
                lubjj = jj;
                break;
            }
            int area = (ii+1-i)*(jj+1-j);
            if (area > maxarea) {
                maxarea = area;
                maxii = ii;
                maxjj = jj;
            }
        }
    }
    for (int ii = i; ii <= maxii; ii++) {
        for (int jj = j; jj <= maxjj; jj++) {
            mark[ii*height+jj] = true;
        }
    }
    ret->push_back(TerrainChunk(type, i, j, maxii, maxjj, width, height));
}

std::list<TerrainChunk> TerrainChunk::make_chunk_list(Terrain* ts, int width, int height) {
    bool* mark = new bool[width*height];
    bzero(mark, sizeof(bool)*width*height);

    std::list<TerrainChunk> ret;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            make_chunk(ts, width, height, mark, &ret, ts[i*height+j].type(), i, j);
        }
    }

    delete[] mark;
    return ret;
}


static GLuint rock_texid = 0;

void load_terrain_textures() {
    glGenTextures(1, &rock_texid);
    SDL_Surface* rocky = IMG_Load("res/rocky.jpg");
    if (rocky == 0) abort();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rock_texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rocky->w, rocky->h, 0, 
                GL_RGB, GL_UNSIGNED_BYTE, rocky->pixels);
}


static void draw_air(vec2 ll, vec2 ur, vec2 boxll, vec2 boxur) {
    const double lor = 0.8, log = 0.8, lob = 0.9;
    const double hir = 0.1, hig = 0.4, hib = 0.75;
    double lh = (boxll.y - ll.y)/(ur.y-ll.y);
    double hh = (boxur.y - ll.y)/(ur.y-ll.y);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
        glColor3f(lh*hir+(1-lh)*lor, lh*hig+(1-lh)*log, lh*hib+(1-lh)*lob);
        glVertex2f(boxll.x, boxll.y);
        glVertex2f(boxur.x, boxll.y);
        glColor3f(hh*hir+(1-hh)*lor, hh*hig+(1-hh)*log, hh*hib+(1-hh)*lob);
        glVertex2f(boxur.x, boxur.y);
        glVertex2f(boxll.x, boxur.y);
    glEnd();
}

static void draw_rock(vec2 ll, vec2 ur, vec2 boxll, vec2 boxur) {
    const double scalex = 10;
    const double scaley = 10;
    glBindTexture(GL_TEXTURE_2D, rock_texid);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glTexCoord2f(boxll.x/scalex, boxll.y/scaley);  glVertex2f(boxll.x, boxll.y);
        glTexCoord2f(boxur.x/scalex, boxll.y/scaley);  glVertex2f(boxur.x, boxll.y);
        glTexCoord2f(boxur.x/scalex, boxur.y/scaley);  glVertex2f(boxur.x, boxur.y);
        glTexCoord2f(boxll.x/scalex, boxur.y/scaley);  glVertex2f(boxll.x, boxur.y);
    glEnd();
}

void TerrainChunk::draw(vec2 ll, vec2 ur) const {
    vec2 boxll( (ur.x - ll.x) * double(minx_)/width_    + ll.x
              , (ur.y - ll.y) * double(miny_)/height_   + ll.y);
    vec2 boxur( (ur.x - ll.x) * double(maxx_+1)/width_  + ll.x
              , (ur.y - ll.y) * double(maxy_+1)/height_ + ll.y);
    switch (type_) {
        case Terrain::AIR:
            draw_air(ll, ur, boxll, boxur);
            break;
        case Terrain::ROCK:
            draw_rock(ll, ur, boxll, boxur);
            break;
        default:
            break;
    }
}
