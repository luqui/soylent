#include <windows.h>
#include <GL/gl.h>
#include "SDL.h"
#include "chipmunk.h"

void init()
{
	cpInitChipmunk();
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);
	
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
	glOrtho(-320.0, 320.0, -240.0, 240.0, -1.0, 1.0);
	glTranslatef(0.5, 0.5, 0.0);
}

int main(int argc, char **argv)
{
	init();

	return 0;
}
