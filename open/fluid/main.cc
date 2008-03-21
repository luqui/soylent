#if defined(WIN32)
#  include <windows.h>
#endif

#define GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>
#include <glh/glh_glut.h>
#include <glh/glh_glut_text.h>
#include "tweak.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <list>
#include <cmath>
#include <iostream>
#include <sstream>
#include <time.h>

#include <Cg/cgGL.h>

#include "flo.h"

#include "vec.h"
#include "FluidGrid.h"
#include "Texture.h"
#include "drawing.h"
#include "Menu.h"
#include "Timer.h"
#include "GameMode.h"
#include "Input.h"
#include "Jukebox.h"

// Algorithms from "Real Time Fluid Dynamics for Games" by Jos Stam.

// ******GPGPU Crap**********
Jukebox *JUKEBOX = 0;
Flo  *g_pFlo = 0;
bool  g_bVectorVisualizationAvailable = false;
CGcontext g_cgContext;
int   g_iFloWidth  = W;
int   g_iFloHeight = H;
bool b[256];

// Booleans for simulation behavior
Flo::DisplayMode g_displayMode = Flo::DISPLAY_INK;
Flo::VectorDisplayMode g_vectorDisplayMode = Flo::VECTOR_DISPLAY_NONE;
Flo::DisplayMode g_vectorField = Flo::DISPLAY_VELOCITY;

// parameters
float g_rVCScale          = 0.025f;
int   g_iNumPoissonSteps  = 50;

float g_rViscosity        = 0;
float g_rInkRGB[3]        = { 0.54f, 0.2f, 0.0f };
float g_rInkLongevity     = 0.999;
float g_rBrushRadius      = 0.1;
float g_rTimestep         = 1;
float g_rGridScale        = 1;
int   g_iPointSize        = 3;
int   g_iLineWidth        = 2;
float g_rVectorScale      = 0.05;
int   g_iPointStride      = 4;

// benchmark globals
float g_rLastTime = 0;
int   g_iFrameCount = 0;
int   g_iTimeDuration = 100;
bool  g_bExitAfterBenchmark = false;
bool  g_bQuiet = false;
bool  g_bStartup = true;
// *********End of GPGPU Crap*************

using std::list;

static int XRES = 1024;
static int YRES = 768;

float TIMESTEP;
bool QUIT = false;


static FontRenderer* FONTRENDERER;

vec MOUSE;
vec JOYSTICK;

//This can go away soon
void addUI(char key, string description, bool initialValue = false)
{
	b[key] = initialValue;
}
//end of crap

void getExtensions(void)
{
	//printf("%s\n", glGetString(GL_EXTENSIONS) );

	if (!glh_init_extensions("GL_ARB_multitexture "
		"GL_VERSION_1_4 "
		//"GL_EXT_framebuffer_object "
		"WGL_ARB_pbuffer "
		"WGL_ARB_pixel_format "
		"WGL_ARB_render_texture "
		)) 
	{
		fprintf(stderr, "Error - required extensions were not supported: %s", glh_get_unsupported_extensions());
		exit(1);
	}

	if (!glh_init_extensions("GL_NV_float_buffer ")) {
		if (!glh_init_extensions("GL_ATI_texture_float ")) {
			fprintf(stderr, "Error - neither GL_NV_float_buffer nor GL_ATI_texture_float are supported. ");
			exit(1);
		}
	}

    if (!glh_init_extensions("GL_ARB_vertex_buffer_object "
                             "GL_NV_vertex_program3 "))
    {
        g_bVectorVisualizationAvailable = false;
    }
}

void cgErrorCallback()
{
	CGerror lastError = cgGetError();

	if(lastError)
	{
		printf("%s\n\n", cgGetErrorString(lastError));
		printf("%s\n", cgGetLastListing(g_cgContext));
		printf("Cg error, exiting...\n");
		exit(1);
	}
}

void set_resolution()
{
#ifdef NDEBUG
	SDL_Surface* screen = 
		SDL_SetVideoMode(XRES, YRES, 0, SDL_FULLSCREEN | SDL_OPENGL);
#else
	SDL_Surface* screen =
		SDL_SetVideoMode(XRES, YRES, 0, SDL_OPENGL);
#endif
	if (screen == 0) {
		std::cerr << "Failed to set video mode: " << SDL_GetError();
		exit(1);
	}

	glViewport(0,0,XRES,YRES);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, W, 0, H);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//***gpgpu fluid stuff follows below***
	getExtensions();

	cgSetErrorCallback(cgErrorCallback);
	
	if(g_cgContext) {
		cgDestroyContext(g_cgContext);
	}
	g_cgContext = cgCreateContext();
	glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);

	// Create and initialize the Flo simulator object
	delete g_pFlo;
	g_pFlo = new Flo(g_iFloWidth, g_iFloHeight);
	g_pFlo->Initialize(g_cgContext, g_bVectorVisualizationAvailable);
	g_pFlo->EnableTextureUpdates(b['U']);
	
	if(b['o']) {
		g_pFlo->EnableArbitraryBC(b['o']);
	}


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	
}

void init_sdl() 
{
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) == -1) {
		std::cout << "Couldn't initialize SDL: " << SDL_GetError() << "\n";
	}

	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	set_resolution();

	int a;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &a);

	// XXX we should make sure that we have at least one bit of this

	int sticks = SDL_NumJoysticks();
	for(int i = 0; i < sticks; i++) {
		if(SDL_JoystickOpen(i)) {
			SDL_JoystickEventState(1);
		}
	}

	SDL_ShowCursor(0);

	FONTRENDERER = new FontRenderer;
}

void init_music() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		std::cout << "Error initializing SDL_mixer: " << Mix_GetError() << "\n";
	}

	JUKEBOX = new Jukebox();

	for (int i = 0; i < NUM_SONGS; i++) {
		JUKEBOX->add_song(SONGS[i]);
	}
}

void events(GameMode* game) 
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (game && game->events(&e)) continue;
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
			QUIT = true;
		}
	}
}

void run(GameMode* game)
{
	Timer fpstimer;
	int frames = 0;
	float fps = 0;
	Timer timer;

	MOUSE = vec();
	QUIT = false;
	while (!QUIT) {
		events(game);
		game->step();

		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		game->draw();

		{  // draw framerate
			glColor3f(1,1,1);
			glRasterPos2f(0,1);
			std::stringstream ss;
			ss << int(fps+0.5) << " FHz";
			draw_string(ss.str());
		}

		SDL_GL_SwapBuffers();

		frames++;
		if (frames % 30 == 0) {
			fps = 30 / fpstimer.get_diff();
			std::cout << "FPS: " << fps << "\n";
		}
		
		TIMESTEP = timer.get_diff();
		if (TIMESTEP < DT/TIMESCALE) {
			SDL_Delay(int(1000*(DT/TIMESCALE - TIMESTEP)));
			timer.get_diff();  // discard that time
		}

		SDL_Delay(0);
	}
}

class SandboxMenuItem: public MenuItem {
public:
	SandboxMenuItem() : MenuItem("Sandbox") { }
	void action(Menu* m) {
		g_pFlo->Reset(true);
		float pos[2] = {0, 0};
		g_pFlo->DrawBoundary(pos, 0, false); // XXX this does something that finishes clearing the boundary

		std::list<Input*> inputs;
		inputs.push_back(new WASDMouseInput);
		inputs.push_back(new DualInput(1));
		GameMode* mode = make_SandboxMode(new AddInput(inputs));
		run(mode);
		delete mode;
		m->reinit();
	}
	bool color() { return false; }
};

class GalagaMenuItem : public MenuItem {
public:
	GalagaMenuItem() : MenuItem("One Player") { }
	void action(Menu* m) {
		g_pFlo->Reset(true);
		float pos[2] = { 0, 0 };
		g_pFlo->DrawBoundary(pos, 0, false); // XXX this does something that finishes clearing the boundary

		std::list<Input*> inputs;
		inputs.push_back(new WASDInput);
		inputs.push_back(new ArrowsInput);
		inputs.push_back(new JoystickInput(0));
		inputs.push_back(new JoystickInput(1));
		inputs.push_back(new MouseInput);
		GameMode* mode = make_GalagaMode(new AddInput(inputs));
		run(mode);
		delete mode;
		m->reinit();
	}
	bool color() { return false; }
};

class DuelMenuItem : public MenuItem {
public:
	DuelMenuItem() : MenuItem("Two Players") { }
	void action(Menu* m) { 
		g_pFlo->Reset(true);
		float pos[2] = { 0, 0 };
		g_pFlo->DrawBoundary(pos, 0, false); // XXX this does something that finishes clearing the boundary

		std::list<Input*> posinputs;
		posinputs.push_back(new WASDInput);
		posinputs.push_back(new JoystickInput(0));
		std::list<Input*> neginputs;
		neginputs.push_back(new ArrowsInput);
		neginputs.push_back(new MouseInput);
		neginputs.push_back(new JoystickInput(1));
		GameMode* mode = make_DuelMode(new AddInput(posinputs), new AddInput(neginputs));
		run(mode); 
		delete mode;
		m->reinit(); 
	}
	bool color() { return true; }
};

class ResolutionMenuItem : public MenuItem {
public:
	ResolutionMenuItem() : MenuItem(""), modes_(SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_OPENGL)) { 
		for (idx_ = 0; modes_[idx_]; idx_++) {
			if (modes_[idx_]->w == XRES && modes_[idx_]->h == YRES) {
				break;
			}
		}
		if (!modes_[idx_]) idx_ = 0;
		parity_ = (idx_+1) % 2;
	}

	string get_text() const {
		std::stringstream ss;
		ss << modes_[idx_]->w << "x" << modes_[idx_]->h;
		return ss.str();
	}

	void action(Menu*) {
		XRES = modes_[idx_]->w;
		YRES = modes_[idx_]->h;
		set_resolution();
	}
	
	void right_action(Menu*) {
		if (idx_ == 0) return;
		idx_--;
	}

	void left_action(Menu*) {
		if (!modes_[idx_+1]) return;
		idx_++;
	}
	
	bool color() { return (idx_ + parity_) % 2; }
private:
	SDL_Rect** modes_;
	int idx_;
	int parity_;
};

class OptionsMenuItem : public MenuItem {
public:
	OptionsMenuItem() : MenuItem("Options"), options_menu(FONTRENDERER) {
		options_menu.add_item(new ResolutionMenuItem);
	}
	void action(Menu* m) {
		options_menu.run();
	}
	bool color() { return false; }
private:
	Menu options_menu;
};

class HighScoresMenuItem : public MenuItem {
public:
	HighScoresMenuItem() : MenuItem("High Scores") { }
	void action(Menu* m) {
		GameMode* mode = make_HighScoreMode();
		run(mode);
		delete mode;
		m->reinit();
	}
	bool color() { return true; }
};

class QuitMenuItem : public MenuItem {
public:
	QuitMenuItem() : MenuItem("Quit") { }
	void action(Menu*) { SDL_Quit(); exit(0); }
	bool color() { return false; }
};

int main(int argc, char** argv)
{
	srand(time(NULL));

	//lets get rid of this soon
	addUI('!', "Toggle FPS Display", false);
    addUI('`', "Toggle Sliders", false);
    addUI('?', "Print help text");
    addUI('1', "Enable jet 1", true);
    addUI('2', "Enable jet 2", true);
    addUI('3', "Enable jet 3", true);
    addUI('4', "Enable jet 4", true);
    addUI('b', "Toggle bilinear interpolation", true);    
    addUI('c', "Toggle ink color cycling", false);    
    addUI('C', "Toggle pressure Clears", false);
    addUI('D', "Toggle display updates", true);
    addUI('j', "Fire ink jets");
    addUI('L', "Time performance", false);
    addUI('m', "Toggle 8-bit display texture (fastest)", false);
    addUI('o', "Enable painting interior obstacles", true); 
    addUI('p', "Toggle Pause", false);
    addUI('.', "Take a single step");
    addUI('r', "Reset fluid");
    addUI('R', "Reset fluid and obstacles");
    addUI('t', "Cycle display field");
    addUI('U', "Toggle texture Updates", true);
    addUI('v', "Toggle vorticity computation", false);
    addUI('w', "Toggle white background", false);
    addUI('z', "Toggle zcull optimization", false);
    addUI('f', "Toggle Vector Field Visualization", false);
    addUI('F', "Cycle Vector Field Point Stride", false);
    addUI('g', "Cycle texture to use for vector field", false);
    addUI('0', "Toggle fluid textures", true);
	//end of shit
	
	init_sdl();
	init_music();
	JUKEBOX->play();

	Menu main_menu(FONTRENDERER);
	
	main_menu.add_item(new SandboxMenuItem);
	main_menu.add_item(new GalagaMenuItem);
	main_menu.add_item(new DuelMenuItem);
	main_menu.add_item(new OptionsMenuItem);
	main_menu.add_item(new HighScoresMenuItem);
	main_menu.add_item(new QuitMenuItem);

	main_menu.run();
	
	delete FONTRENDERER;
	JUKEBOX->stop();
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}
