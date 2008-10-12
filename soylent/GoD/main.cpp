#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <list>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <FTGL/ftgl.h>
#include "SDL.h"
#include "chipmunk.h"
#include <string>
#include <sstream>

#include "avatar.h"

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FRAME_CAP = 60;

int ticks = 0;
cpSpace *space;
cpBody *staticBody;

Avatar *avatar;
list<Entity*> entities;
Input_Manager *inputManager;

FTFont *font;
int fps;

void init()
{
	cpInitChipmunk();

	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_OPENGL);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPointSize(2.5);
	
    glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
    glLineWidth(2.5f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-SCREEN_WIDTH/2.0, SCREEN_WIDTH/2.0, -SCREEN_HEIGHT/2.0, SCREEN_HEIGHT/2.0);

	inputManager = new Input_Manager();

	font = new FTBufferFont("Resources/zrnic___.ttf");
	font->FaceSize(30);
    font->CharMap(ft_encoding_unicode);
}

#define NUM_VERTS 5
void setup()
{
	staticBody = cpBodyNew(INFINITY, INFINITY);
	
	cpResetShapeIdCounter();
	space = cpSpaceNew();
	space->iterations = 10;//default is 10
	//space->damping = 0.9f;
	//space->gravity = cpv(0, -100);

	//The spatial hashes used by Chipmunk’s collision detection are fairly size sensitive. 
	//dim is the size of the hash cells. Setting dim to the average objects size is likely to give the best performance.
	//count is the suggested minimum number of cells in the hash table. Bigger is better, but only to a point. 
	//Setting count to ~10x the number of objects in the hash is probably a good starting point.
	//By default, dim is 100.0, and count is 1000.
	cpSpaceResizeStaticHash(space, 40.0, 999);
	cpSpaceResizeActiveHash(space, 30.0, 2999);

	cpBody *body;
	cpShape *shape;
	Entity *entity;

	//we are making polygons (pentagons)
	cpVect verts[NUM_VERTS];
	for(int i=0; i<NUM_VERTS; i++){
		cpFloat angle = -2*(cpFloat)M_PI*i/((cpFloat) NUM_VERTS);
		verts[i] = cpv(10*cos(angle), 10*sin(angle));
	}

	const int NUM_POLYS = 3;
	for(int i=0; i<NUM_POLYS; i++){
		body = cpBodyNew(1.0, cpMomentForPoly(1.0, NUM_VERTS, verts, cpvzero));
//		body = cpBodyNew(1.0, cpMomentForCircle(1.0, 0.0, 10.0, cpvzero));
		body->p = cpv((i+1)*SCREEN_WIDTH/float(NUM_POLYS+1) - SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/6.0f);
		//cpSpaceAddBody(space, body);
		shape = cpPolyShapeNew(body, NUM_VERTS, verts, cpvzero);
//		shape = cpCircleShapeNew(body, 10.0, cpvzero);
		shape->e = 0.0f; shape->u = 0.04f;
		//cpSpaceAddShape(space, shape);
		
		entity = new Entity(space, body, shape, 10);

		entities.push_back(entity);
	}

	//Avatar
	body = cpBodyNew(1.0, cpMomentForPoly(1.0, NUM_VERTS, verts, cpvzero));
	body->p = cpv(0, -SCREEN_HEIGHT/6);
	shape = cpPolyShapeNew(body, NUM_VERTS, verts, cpvzero);
	shape->e = 0.0f; shape->u = 0.04f;
	avatar = new Avatar(space, body, shape, 10);
	entities.push_back(avatar);

	const int numEvents = 8;
	SDL_Event events[numEvents];
	events[0].key.keysym.sym = events[4].key.keysym.sym = SDLK_w;
	events[1].key.keysym.sym = events[5].key.keysym.sym = SDLK_s;
	events[2].key.keysym.sym = events[6].key.keysym.sym = SDLK_a;
	events[3].key.keysym.sym = events[7].key.keysym.sym = SDLK_d;
	for(int i = 0; i < numEvents; i++) {
		events[i].type = (i < 4) ? events[i].type = SDL_KEYDOWN : events[i].type = SDL_KEYUP;
	}

	Thrust_Command *thruster = new Thrust_Command(avatar, events, numEvents);
	inputManager->Register(thruster);

}

int G_LastMouseX = 0;
int G_LastMouseY = 0;
extern Uint32 G_MM_Update;
void logic ()
{
	float x_vel, y_vel;
	int MouseX, MouseY;

	if(SDL_GetTicks() - G_MM_Update > 10) {
		G_MM_Update = SDL_GetTicks();

		int state =	SDL_GetMouseState(&MouseX, &MouseY);
		x_vel = -4.0f*(G_LastMouseX - MouseX);
		y_vel =  4.0f*(G_LastMouseY - MouseY);

		if(state & SDL_BUTTON(1)) {
			cpBody *body;
			cpShape *shape;

			float radius = 3;
			float mass = 0.0001f;
			
			body = cpBodyNew(mass, cpMomentForCircle(mass, 0.0, radius, cpvzero));
			body->p = cpv(MouseX - SCREEN_WIDTH / 2.0, -MouseY + SCREEN_HEIGHT / 2.0);
			body->v = cpv(x_vel, y_vel);
			cpSpaceAddBody(space, body);
			shape = cpCircleShapeNew(body, radius, cpvzero);
			shape->e = 0.0f; shape->u = 1.0f;
			shape->group = 1;
			cpSpaceAddShape(space, shape);
		}

		G_LastMouseX = MouseX;
		G_LastMouseY = MouseY;
	}
}

static void
eachBody(cpBody *body, void *unused)
{
	/*if(body->data != NULL) {
		if(body->data == avatar) avatar->Update();
	}*/

	if(body->p.x > SCREEN_WIDTH/2.0) {
		body->p.x = -SCREEN_WIDTH/2.0;
	}
	if(body->p.x < -SCREEN_WIDTH/2.0) {
		body->p.x = SCREEN_WIDTH/2.0;
	}
	if(body->p.y > SCREEN_HEIGHT/2.0) {
		body->p.y = -SCREEN_HEIGHT/2.0;
	}
	if(body->p.y < -SCREEN_HEIGHT/2.0) {
		body->p.y = SCREEN_HEIGHT/2.0;
	}
}

void update()
{
	list<Entity*>::iterator it;
	for( it=entities.begin(); it != entities.end(); it++ ) 
	{
		(*it)->Update();
	}

	int steps = 1;
	cpFloat dt = 1.0/FRAME_CAP/(cpFloat)steps;
	
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
	glColor4f(0.0, 1.0, 0.0, 0.3);
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
	glColor3f(1.0, 1.0, 1.0);
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

static void 
drawObject(void *ptr, void *unused)
{
	cpShape *shape = (cpShape *)ptr;
	switch(shape->klass->type){
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);

	cpSpaceHashEach(space->activeShapes, &drawObject, NULL);
	cpSpaceHashEach(space->staticShapes, &drawObject, NULL);

	glColor3f(0.0f, 0.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-SCREEN_WIDTH/2.1f, -SCREEN_HEIGHT/2.1f, 0.0f);
	std::string text;
	std::stringstream ss;
	ss << fps;
	ss >> text;
	text.append(" fps");
	font->Render(text.c_str());
	
	SDL_GL_SwapBuffers();
}

void run()
{
    Uint32 lastTicks = SDL_GetTicks();
	int frame_counter = 0;
	Uint32 frame_timer = SDL_GetTicks();
	while(true)
	{
		//cap the FPS
		Uint32 dt = SDL_GetTicks() - lastTicks;
        lastTicks = SDL_GetTicks();
		int delay = 1000.0/(FRAME_CAP + 1) - dt;
		if(delay > 0) SDL_Delay(delay);

		//calculate FPS
		frame_counter++;
		if(SDL_GetTicks() - frame_timer > 500) {
			fps = frame_counter / ((SDL_GetTicks() - frame_timer) / 1000.0);
			frame_counter = 0;
			frame_timer = SDL_GetTicks();
		}

		//process events
		if(inputManager->Manage() == 0) return;

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

	SDL_Quit();

	delete avatar;

}

int main(int argc, char **argv)
{
	init();
	setup();
	run();
	quit();
	return 0;
}
