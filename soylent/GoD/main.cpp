#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"
#include "chipmunk.h"

#include "avatar.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int ticks = 0;
cpSpace *space;
cpBody *staticBody;

Avatar *avatar;

void init()
{
	cpInitChipmunk();

	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_OPENGL);
	
	glClearColor(1.0, 1.0, 1.0, 0.0);
	/*glPointSize(3.0);
	
    glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
    glLineWidth(2.5f);*/

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-SCREEN_WIDTH/2.0, SCREEN_WIDTH/2.0, -SCREEN_HEIGHT/2.0, SCREEN_HEIGHT/2.0);
	//don't think we need this: glTranslatef(0.5, 0.5, 0.0);
}

#define NUM_VERTS 5
void setup()
{
	staticBody = cpBodyNew(INFINITY, INFINITY);
	
	cpResetShapeIdCounter();
	space = cpSpaceNew();
	space->iterations = 10;//default is 10
	space->gravity = cpv(0, -100);

	//The spatial hashes used by Chipmunk’s collision detection are fairly size sensitive. 
	//dim is the size of the hash cells. Setting dim to the average objects size is likely to give the best performance.
	//count is the suggested minimum number of cells in the hash table. Bigger is better, but only to a point. 
	//Setting count to ~10x the number of objects in the hash is probably a good starting point.
	//By default, dim is 100.0, and count is 1000.
	cpSpaceResizeStaticHash(space, 40.0, 999);
	cpSpaceResizeActiveHash(space, 30.0, 2999);

	cpBody *body;
	cpShape *shape;

	//we are making polygons (pentagons)
	cpVect verts[NUM_VERTS];
	for(int i=0; i<NUM_VERTS; i++){
		cpFloat angle = -2*(cpFloat)M_PI*i/((cpFloat) NUM_VERTS);
		verts[i] = cpv(10*cos(angle), 10*sin(angle));
	}

	for(int i=0; i<3; i++){
		body = cpBodyNew(1.0, cpMomentForPoly(1.0, NUM_VERTS, verts, cpvzero));
//		body = cpBodyNew(1.0, cpMomentForCircle(1.0, 0.0, 10.0, cpvzero));
		cpFloat x = rand()/(cpFloat)RAND_MAX*640 - 320;
		body->p = cpv(x, 350);
		cpSpaceAddBody(space, body);
		shape = cpPolyShapeNew(body, NUM_VERTS, verts, cpvzero);
//		shape = cpCircleShapeNew(body, 10.0, cpvzero);
		shape->e = 0.0; shape->u = 0.04;
		cpSpaceAddShape(space, shape);
	}

	body->p = cpv(0, 0);
	avatar = new Avatar(space, body, shape);
}

//Handle Keyboard and Mouse events.
int event_loop()
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		SDLKey key = e.key.keysym.sym;
		switch(e.type) 
		{
		case SDL_KEYDOWN:
			if(key == SDLK_ESCAPE) return 0;
			else if(key == SDLK_w)
			{
			}
			else if(key == SDLK_s)
			{
			}
			else if(key == SDLK_a)
			{}
			else if(key == SDLK_d) {}
			break;
		default:
			break;
		}
	}
	return 1;
}

void logic ()
{

}

static void
eachBody(cpBody *body, void *unused)
{
	if(body->p.y < -260 || fabsf(body->p.x) > 340){
		cpFloat x = rand()/(cpFloat)RAND_MAX*640 - 320;
		body->p = cpv(x, 260);
	}
}

void update()
{
	int steps = 1;
	cpFloat dt = 1.0/60.0/(cpFloat)steps;
	
	for(int i=0; i<steps; i++){
		cpSpaceStep(space, dt);
		cpSpaceEachBody(space, &eachBody, NULL);
	}
}

void drawCircle(cpFloat x, cpFloat y, cpFloat r, cpFloat a)
{
	const int segs = 15;
	const cpFloat coef = 2.0*M_PI/(cpFloat)segs;
	
	glBegin(GL_LINE_STRIP); {
		for(int n = 0; n <= segs; n++){
			cpFloat rads = n*coef;
			glVertex2f(r*cos(rads + a) + x, r*sin(rads + a) + y);
		}
		glVertex2f(x,y);
	} glEnd();
}

void drawCircleShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpCircleShape *circle = (cpCircleShape *)shape;
	cpVect c = cpvadd(body->p, cpvrotate(circle->c, body->rot));
	drawCircle(c.x, c.y, circle->r, body->a);
}

void drawSegmentShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpSegmentShape *seg = (cpSegmentShape *)shape;
	cpVect a = cpvadd(body->p, cpvrotate(seg->a, body->rot));
	cpVect b = cpvadd(body->p, cpvrotate(seg->b, body->rot));
	
	glBegin(GL_LINES); {
		glVertex2f(a.x, a.y);
		glVertex2f(b.x, b.y);
	} glEnd();
}

void drawPolyShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpPolyShape *poly = (cpPolyShape *)shape;
	
	int num = poly->numVerts;
	cpVect *verts = poly->verts;
	
	glBegin(GL_LINE_LOOP);
	for(int i=0; i<num; i++){
		cpVect v = cpvadd(body->p, cpvrotate(verts[i], body->rot));
		glVertex2f(v.x, v.y);
	} glEnd();
}

void drawObject(void *ptr, void *unused)
{
	cpShape *shape = (cpShape *)ptr;
	switch(shape->type){
		case CP_CIRCLE_SHAPE:
			drawCircleShape(shape);
			break;
		case CP_SEGMENT_SHAPE:
			drawSegmentShape(shape);
			break;
		case CP_POLY_SHAPE:
			drawPolyShape(shape);
			break;
		default:
			printf("Bad enumeration in drawObject().\n");
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0, 0.0, 0.0);
	cpSpaceHashEach(space->activeShapes, &drawObject, NULL);
	cpSpaceHashEach(space->staticShapes, &drawObject, NULL);
	
	SDL_GL_SwapBuffers();
}

void run()
{
	while(true)
	{	
		//process events
		if(event_loop() == 0) return;

		//perform game logic
		logic();

		//update the simulation
		update();

		//draw stuff
		draw();
	}
}

void quit()
{
	cpSpaceFreeChildren(space);
	cpSpaceFree(space);
	
	cpBodyFree(staticBody);

	delete avatar;
}

int main(int argc, char **argv)
{
	init();
	setup();
	run();
	//quit();
	return 0;
}
