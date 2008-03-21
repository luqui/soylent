#ifndef __DRAWQUEUE_H__
#define __DRAWQUEUE_H__

#include <queue>
#include <GL/gl.h>
#include <GL/glu.h>

class Drawable {
public:
    virtual ~Drawable() { }

    virtual void draw() const;

    void _draw_after() const {
        glLoadMatrixd(matrix_);
        draw_delayed();
    }
    
    virtual void draw_delayed() const = 0;
    
private:
    mutable GLdouble matrix_[16];
};

class DrawQueue {
public:
    void push(const Drawable* d) {
        q_.push(d);
    }

    void draw() {
        glDepthMask(GL_FALSE);
        while (!q_.empty()) {
            const Drawable* d = q_.front();
            d->_draw_after();
            q_.pop();
        }
        glDepthMask(GL_TRUE);
    }
private:
    typedef std::queue<const Drawable*> q_t;
    q_t q_;
};

extern DrawQueue DRAWQUEUE;

inline void Drawable::draw() const {
    glGetDoublev(GL_MODELVIEW_MATRIX, matrix_);
    DRAWQUEUE.push(this);
}

#endif
