#include <soy/init.h>
#include <soy/Viewport.h>
#include <soy/Timer.h>
#include <soy/util.h>
#include "game.h"
#include "PowerComponent.h"
#include "Pickable.h"

const double STEP = 1/60.0;
static SoyInit INIT;
static Viewport VIEW(vec2(0,0), vec2(16,12));
dWorldID WORLD;
dJointGroupID CONTACTS;
dSpaceID SPACE;

objects_t OBJECTS;
templates_t TEMPLATES;

static void init()
{
    srand(time(NULL));
    
    INIT.init();
    WORLD = dWorldCreate();
    CONTACTS = dJointGroupCreate(0);
    SPACE = dSimpleSpaceCreate(0);

    dCreatePlane(SPACE, -1, 0, 0, -8);
    dCreatePlane(SPACE,  1, 0, 0, -8);
    dCreatePlane(SPACE,  0,-1, 0, -6);
    dCreatePlane(SPACE,  0, 1, 0, -6);
    
    VIEW.activate();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    TEMPLATES.insert(new PowerComponentTemplate);
}

static void draw()
{
    for (objects_t::const_iterator i = OBJECTS.begin();
            i != OBJECTS.end(); ++i) {
        (*i)->draw();
    }
}

static void collision_callback(void* data, dGeomID ga, dGeomID gb)
{
    if (dGeomIsSpace(ga) || dGeomIsSpace(gb)) {
        dSpaceCollide2(ga, gb, data, &collision_callback);

        if (dGeomIsSpace(ga)) {
            dSpaceCollide(dSpaceID(ga), data, &collision_callback);
        }
        if (dGeomIsSpace(gb)) {
            dSpaceCollide(dSpaceID(gb), data, &collision_callback);
        }
    }
    else {
        const int max_contacts = 4;
        dContactGeom contacts[max_contacts];
        int ncontacts = dCollide(ga, gb, max_contacts, contacts, sizeof(dContactGeom));

        for (int i = 0; i < ncontacts; i++) {
            dContact contact;
            contact.surface.mode = dContactBounce;
            contact.surface.mu = 0.3;
            contact.surface.bounce = 0.5;
            contact.surface.bounce_vel = 0.1;
            contact.geom = contacts[i];

            dJointID ctct = dJointCreateContact(WORLD, CONTACTS, &contact);
            dJointAttach(ctct, dGeomGetBody(ga), dGeomGetBody(gb));
        }
    }
}


static void step()
{
    for (objects_t::iterator i = OBJECTS.begin();
            i != OBJECTS.end(); ++i) {
        (*i)->step();
    }

    dSpaceCollide(SPACE, NULL, &collision_callback);
    dWorldQuickStep(WORLD, STEP);
    dJointGroupEmpty(CONTACTS);
}

static void events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            INIT.quit();
            exit(0);
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_n) {
            Component* comp = (*TEMPLATES.begin())->create();
            comp->set_position(vec2(randrange(-7,7), randrange(-5,5)));

            OBJECTS.insert(comp);
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            // allocate a buffer to place our hits
            const int bufsize = 128;
            GLuint hits[bufsize];
            glSelectBuffer(bufsize, hits);

            glRenderMode(GL_SELECT);

            // make the projection matrix a 1x1-pixel area of the screen:
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            //   get the viewport so that we can call gluPickMatrix
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            gluPickMatrix(e.button.x, viewport[3] - e.button.y, 1, 1, viewport);
            VIEW.multiply();

            // draw the scene to fill the hit buffer
            glMatrixMode(GL_MODELVIEW);
            glInitNames();
            draw();

            // return to our old projection matrix
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glFlush();  // ???
            
            // dispatch the onpicks
            GLuint* hitptr = hits;
            int numhits = glRenderMode(GL_RENDER);
            while (numhits --> 0) {
                int names = *hitptr++;
                hitptr++;  // min depth
                hitptr++;  // max depth
                for (int i = 0; i < names; i++) {
                    reinterpret_cast<Pickable*>(*hitptr++)->on_pick();
                }
            }
        }
    }
}

int main()
{
    init();

    FrameRateLockTimer timer(STEP);
    while (true) {
        events();
        
        glClear(GL_COLOR_BUFFER_BIT);
        draw();
        SDL_GL_SwapBuffers();

        step();
        timer.lock();
    }
}
