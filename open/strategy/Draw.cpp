#include "Draw.h"
#include "Widget.h"


void Artist::enqueue(const tvec& ref, const Widget* wid) {
    queue_.insert(
        new DrawElem (wid, wid->gheight(ref), wid->lheight(ref), wid->pos(ref))
    );
}

void Artist::render() const {
    for (queue_t::const_iterator i = queue_.begin();
            i != queue_.end(); ++i) {
        glPushMatrix();
            glTranslated((*i)->pos.x, (*i)->pos.y, 0);
            (*i)->widget->render();
        glPopMatrix();
    }
}

void Artist::clear() {
    queue_.clear();
}

void Draw::texture_square()
{
    glBegin(GL_QUADS);
        glTexCoord2d(0, 1);  glVertex2d(0, 0);
        glTexCoord2d(0, 0);  glVertex2d(0, 1);
        glTexCoord2d(1, 0);  glVertex2d(1, 1);
        glTexCoord2d(1, 1);  glVertex2d(1, 0);
    glEnd();
}
