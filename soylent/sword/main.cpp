#include <soy/init.h>
#include <soy/Timer.h>
#include "InDuelMode.h"
#include <ManyMouseInput.h>
#include <SDLInput.h>
#include "physics.h"
#include "audio.h"
#include "config.h"
#include <fstream>
#include "SinglePlayerMode.h"

SoyInit INIT;
Sound SOUND;

dWorldID WORLD;
dSpaceID COLLIDE_SPACE;
dJointGroupID CONTACT_JOINT_GROUP;

// InputManager INPUT;

DrawQueue DRAWQUEUE;

DrawEye EYE = DRAWEYE_LEFT;

bool gBUseInputNexus(true);
bool USE_STEREO_3D(false);  // screw hungary!
enum ScreenMode 
{
	SmSplit,
	SmGlobal,
	SmPlayer1,
	SmPlayer2
};
ScreenMode gScreenMode(SmSplit);
bool gBSinglePlayer(false);

NVThing gNvtPrefs;

void init() 
{
	// Load up supported stances into the StanceBank.

	StanceBank::SAddStance(new PlaneChudanStance);
	StanceBank::SAddStance(new HemiJabStance);
	StanceBank::SAddStance(new HemiChudanStance);
	StanceBank::SAddStance(new GeneralBlockStance);
	StanceBank::SAddStance(new WristStance);
	StanceBank::SAddStance(new HiltCentricHemis);
	StanceBank::SAddStance(new TipCentricHemis);
	StanceBank::SAddStance(new DeadStance);
	StanceBank::SAddStance(new HorizontalBlockStance);
	StanceBank::SAddStance(new VerticalBlockStance);
	StanceBank::SAddStance(new HiltCentricRel);

	ifstream prefs("swordprefs.txt");
	bool writeprefs(false);
	if (prefs.is_open())
		prefs >> gNvtPrefs;
	else
		writeprefs = true;

	bool lB(false);
	gNvtPrefs.Find("FullScreen", lB);

	gNvtPrefs.Find("Stereo3d", USE_STEREO_3D);

	INIT.set_fullscreen(lB);
	INIT.set_init_flags(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
	INIT.init();

	lB = false;
	gNvtPrefs.Find("AudioEnabled", lB);
	if (lB)
		SOUND.init_audio();

	string screenmode;
	const char *c;
	if (!gNvtPrefs.Find("ScreenMode", c))
	{
		gNvtPrefs.Add("ScreenMode", "Split");
		screenmode = "Split";
	}
	else
		screenmode = c;

	if (screenmode == "Split")
		gScreenMode = SmSplit;
	else if (screenmode == "Global")
		gScreenMode = SmGlobal;
	else if (screenmode == "Player1")
		gScreenMode = SmPlayer1;
	else if (screenmode == "Player2")
		gScreenMode = SmPlayer2;



	if (!gNvtPrefs.Find("UseInputNexus", gBUseInputNexus))
		gNvtPrefs.Add("UseInputNexus", gBUseInputNexus);
	if (!gNvtPrefs.Find("SinglePlayer", gBSinglePlayer))
		gNvtPrefs.Add("SinglePlayer", gBSinglePlayer);

	if (writeprefs)
	{
		// Add some single player stuff.
		NVThing *lNvtSingle = new NVThing;

		SinglePlayerMode::SetupDefaultPrefs(*lNvtSingle);

		gNvtPrefs.Add("SinglePlayerPrefs", lNvtSingle);

		ofstream prefs("swordprefs.txt");
		prefs << gNvtPrefs;
	}

    WORLD = dWorldCreate();
    COLLIDE_SPACE = dSimpleSpaceCreate(0);
    CONTACT_JOINT_GROUP = dJointGroupCreate(0);
    dWorldSetGravity(WORLD, 0, -9.8, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	if (gScreenMode == SmSplit)
		gluPerspective(75.0, (double)INIT.width() * .5/(double)INIT.height(), 1.0, 100.0);
	else
		gluPerspective(75.0, (double)INIT.width()/(double)INIT.height(), 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);

    float fog_color[4] = { 0.1, 0.05, 0, 1 };
    
    glEnable(GL_FOG);
    glFogf(GL_FOG_DENSITY, 0.03);
    glFogfv(GL_FOG_COLOR, fog_color);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    
    float ambient_light[4] = { 0.5, 0.5, 0.5, 1 };
    float diffuse_light[4] = { 0.75, 0.75, 0.75, 1 };
    float light_pos[4]     = { 0, 4, 0, 1 };  // w is 1 for being a point source

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos);

    glEnable(GL_LIGHT1);
}

//class GlobalInputDirect : public Input
//{
//public:
//    void handle_SDL_Event(SDL_Event* e) {
//        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE
//         || e->type == SDL_QUIT) {
//			Mix_CloseAudio();
//            INIT.quit();
//            exit(0);
//        }
//        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_f) {
//            SDL_WM_ToggleFullScreen(INIT.get_surface());
//        }
//    }
//};

//void DirectInputInit()
//{
//    ManyMouse_Init();
//
//    int sticks = SDL_NumJoysticks();
//    for (int i = 0; i < sticks; i++) {
//        SDL_JoystickOpen(i);
//    }
//    SDL_JoystickEventState(1);
//
//    SDL_ShowCursor(0);
//
//	INPUT.add_input(new GlobalInputDirect);
//}

class GlobalInput : public Controllable
{
public:
	GlobalInput()
		:quit_(false)
	{}

	virtual void AddCommandSpecs(CommandSpecContainer *aCsc)
	{
		mUcQuit = aCsc->AddCommandSpec(new CommandSpec("Quit"));
		mUcFullScreen = aCsc->AddCommandSpec(new CommandSpec("Toggle full screen view"));
	}

	virtual void ExecuteCommand (Command *aC)
	{
		if (aC->mUcId == mUcQuit && aC->mFAmt != .0)
		{
			quit_ = true;
            //INIT.quit();
            //exit(0);
		}
		if (aC->mUcId == mUcFullScreen && aC->mFAmt != .0)
		{
            SDL_WM_ToggleFullScreen(INIT.get_surface());
		}
	}

	inline bool ShouldQuit() { return quit_; }

private:
	bool quit_;
	unsigned char mUcQuit;
	unsigned char mUcFullScreen;
};

void SplitScreenDraw(vec3 viewfrom, DuelMode &aDm)
{
	if (gScreenMode == SmSplit)
	{
		glViewport(0, 0, INIT.width() / 2, INIT.height());
		aDm.draw(viewfrom, DuelMode::PPlayer1);
		DRAWQUEUE.draw();

		glViewport(INIT.width() / 2, 0, INIT.width()/2, INIT.height());
		aDm.draw(viewfrom, DuelMode::PPlayer2);
		DRAWQUEUE.draw();

		glViewport(0, 0, INIT.width(), INIT.height());
		aDm.draw_scores();
	}
	else if (gScreenMode == SmGlobal)
	{
		aDm.draw(viewfrom, DuelMode::PGlobal);
		DRAWQUEUE.draw();
		aDm.draw_scores();
	}
	else if (gScreenMode == SmPlayer1)
	{
		aDm.draw(viewfrom, DuelMode::PPlayer1);
		DRAWQUEUE.draw();
		aDm.draw_scores();
	}
	else if (gScreenMode == SmPlayer2)
	{
		aDm.draw(viewfrom, DuelMode::PPlayer2);
		DRAWQUEUE.draw();
		aDm.draw_scores();
	}
}

DuelMode* CreateDm()
{
	if (gBSinglePlayer)
	{
		return new SinglePlayerMode(gNvtPrefs);
	}
	else
	{
		return new DuelMode(gNvtPrefs);
	}
}

int main(int argc, char** argv) 
{
    FrameRateLockTimer timer(DT * MULTISTEP);

    init();

	// ----------------------------------------------------------------
	// Set up game objects, and game input.
	// ----------------------------------------------------------------

	try
	{

	if (gBUseInputNexus)
	{

		// Create the InputNexus object, which handles game input.
		TManyMouseInputNexus<TSdlInputNexus<InputNexus> > lInputNexus;

		// The game mode object, InDuelMode, home of the step() and draw() functions.
		auto_ptr<DuelMode> lDm(CreateDm());		// auto_ptr ensures delete on ftn exit.

		{
			ofstream lOfs("swordprefs.txt");
			lOfs << gNvtPrefs;
			lOfs.close();
		}
		

		// InDuelMode is a controllable that contains a DuelMode ptr.
		InDuelMode lIdm(lDm.get());

		// Set up the input heirarchy.  
		// A FlockController will send all inputs to both the InDuelMode and 
		// GlobalInput objects.
		GlobalInput lGi;
		FlockController lFc;
		lFc.AddControllable(&lGi);
		lFc.AddControllable(&lIdm);

		// Determine what controls are currently available.
		lInputNexus.DiscoverInputSources();

		// Add command specs of the Controllable at the top of the heirarchy.
		// This will add the specs of its sub-controllables.
		lFc.AddCommandSpecs(&lInputNexus);

		string lS(argv[0]);
		lS += ".InputNexus.txt";

		// Read in current command-control associations, if any.
		ifstream lIfs(lS.c_str());
		if (lIfs.is_open())
		{
			lIfs >> lInputNexus;
			lIfs.close();
		}
		
        lInputNexus.Init();

		// Write out combination of active inputsources and current 
		// commands-control associations.  Currently, you have to edit this
		// file in ControlConfig.exe
		ofstream lOfs(lS.c_str());
		lOfs << lInputNexus;
		lOfs.close();

		// ----------------------------------------------------------------
		// Main loop for the game.
		// ----------------------------------------------------------------
        
        glColorMask(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);

		while (!lGi.ShouldQuit()) 
		{
			for (int i = 0; i < MULTISTEP; i++) 
			{
				lInputNexus.ExecuteCommands(&lFc);
				lDm->step();
			}

			vec3 viewfrom;
			if (gScreenMode != SmGlobal)
				viewfrom = vec3(-1.5, 1, 3);
			else
				viewfrom = vec3(.0, .0, .0);

			if (USE_STEREO_3D) 
			{
				glColorMask(1, 0, 0, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				viewfrom.x -= .05;
				SplitScreenDraw(viewfrom, *lDm);

				glColorMask(0, 0, 1, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				viewfrom.x += .1;
				SplitScreenDraw(viewfrom, *lDm);
			}
			else 
			{
				glColorMask(1, 1, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				SplitScreenDraw(viewfrom, *lDm);
			}
			SDL_GL_SwapBuffers();

			timer.lock();
		}
	}
	}
	catch (bburdette::Exception &aE)
	{
		DIE(aE.GetMessage());
	}

	INIT.quit();
	exit(0);

	return 0;
}

