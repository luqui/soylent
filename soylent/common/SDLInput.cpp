#include "SDLInput.h"
//#include "SDL_sysjoystick.h"
//#include "SDL_joystick.h"

// Static stuff.	
Uint8 *SdlKeyIs::sUi8KeyState;
int *SdlKeyIs::sIKeyCount;


void SdlInputNexusA::DiscoverInputSourcesA(InputNexus *aIn)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Add keyboard inputsources.
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	aIn->AddInputSource(new SdlKeyIs(SDLK_UNKNOWN, "UNKNOWN"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_FIRST, "FIRST"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_BACKSPACE, "BACKSPACE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_TAB, "TAB"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_CLEAR, "CLEAR"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RETURN, "RETURN"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_PAUSE, "PAUSE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_ESCAPE, "ESCAPE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_SPACE, "SPACE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_EXCLAIM, "EXCLAIM"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_QUOTEDBL, "QUOTEDBL"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_HASH, "HASH"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_DOLLAR, "DOLLAR"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_AMPERSAND, "AMPERSAND"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_QUOTE, "QUOTE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LEFTPAREN, "LEFTPAREN"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RIGHTPAREN, "RIGHTPAREN"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_ASTERISK, "ASTERISK"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_PLUS, "PLUS"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_COMMA, "COMMA"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_MINUS, "MINUS"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_PERIOD, "PERIOD"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_SLASH, "SLASH"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_0, "0"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_1, "1"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_2, "2"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_3, "3"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_4, "4"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_5, "5"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_6, "6"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_7, "7"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_8, "8"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_9, "9"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_COLON, "COLON"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_SEMICOLON, "SEMICOLON"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LESS, "LESS"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_EQUALS, "EQUALS"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_GREATER, "GREATER"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_QUESTION, "QUESTION"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_AT, "AT	"));
	//   Skip uppercase letters
	aIn->AddInputSource(new SdlKeyIs(SDLK_LEFTBRACKET, "LEFTBRACKET"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_BACKSLASH, "BACKSLASH"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RIGHTBRACKET, "RIGHTBRACKET"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_CARET, "CARET"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_UNDERSCORE, "UNDERSCORE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_BACKQUOTE, "BACKQUOTE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_a, "a"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_b, "b"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_c, "c"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_d, "d"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_e, "e"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_f, "f"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_g, "g"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_h, "h"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_i, "i"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_j, "j"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_k, "k"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_l, "l"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_m, "m"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_n, "n"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_o, "o"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_p, "p"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_q, "q"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_r, "r"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_s, "s"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_t, "t"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_u, "u"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_v, "v"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_w, "w"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_x, "x"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_y, "y"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_z, "z"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_DELETE, "DELETE"));
	// End of ASCII mapped keysyms 

	// International keyboard syms 
	/*
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_0, "WORLD_0"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_1, "WORLD_1"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_2, "WORLD_2"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_3, "WORLD_3"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_4, "WORLD_4"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_5, "WORLD_5"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_6, "WORLD_6"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_7, "WORLD_7"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_8, "WORLD_8"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_9, "WORLD_9"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_10, "WORLD_10"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_11, "WORLD_11"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_12, "WORLD_12"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_13, "WORLD_13"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_14, "WORLD_14"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_15, "WORLD_15"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_16, "WORLD_16"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_17, "WORLD_17"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_18, "WORLD_18"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_19, "WORLD_19"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_20, "WORLD_20"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_21, "WORLD_21"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_22, "WORLD_22"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_23, "WORLD_23"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_24, "WORLD_24"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_25, "WORLD_25"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_26, "WORLD_26"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_27, "WORLD_27"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_28, "WORLD_28"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_29, "WORLD_29"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_30, "WORLD_30"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_31, "WORLD_31"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_32, "WORLD_32"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_33, "WORLD_33"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_34, "WORLD_34"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_35, "WORLD_35"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_36, "WORLD_36"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_37, "WORLD_37"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_38, "WORLD_38"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_39, "WORLD_39"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_40, "WORLD_40"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_41, "WORLD_41"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_42, "WORLD_42"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_43, "WORLD_43"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_44, "WORLD_44"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_45, "WORLD_45"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_46, "WORLD_46"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_47, "WORLD_47"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_48, "WORLD_48"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_49, "WORLD_49"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_50, "WORLD_50"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_51, "WORLD_51"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_52, "WORLD_52"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_53, "WORLD_53"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_54, "WORLD_54"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_55, "WORLD_55"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_56, "WORLD_56"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_57, "WORLD_57"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_58, "WORLD_58"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_59, "WORLD_59"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_60, "WORLD_60"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_61, "WORLD_61"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_62, "WORLD_62"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_63, "WORLD_63"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_64, "WORLD_64"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_65, "WORLD_65"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_66, "WORLD_66"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_67, "WORLD_67"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_68, "WORLD_68"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_69, "WORLD_69"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_70, "WORLD_70"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_71, "WORLD_71"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_72, "WORLD_72"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_73, "WORLD_73"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_74, "WORLD_74"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_75, "WORLD_75"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_76, "WORLD_76"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_77, "WORLD_77"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_78, "WORLD_78"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_79, "WORLD_79"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_80, "WORLD_80"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_81, "WORLD_81"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_82, "WORLD_82"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_83, "WORLD_83"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_84, "WORLD_84"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_85, "WORLD_85"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_86, "WORLD_86"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_87, "WORLD_87"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_88, "WORLD_88"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_89, "WORLD_89"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_90, "WORLD_90"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_91, "WORLD_91"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_92, "WORLD_92"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_93, "WORLD_93"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_94, "WORLD_94"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_WORLD_95, "WORLD_95"));
	*/

	// Numeric keypad 
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP0, "KP0"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP1, "KP1"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP2, "KP2"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP3, "KP3"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP4, "KP4"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP5, "KP5"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP6, "KP6"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP7, "KP7"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP8, "KP8"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP9, "KP9"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_PERIOD, "KP_PERIOD"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_DIVIDE, "KP_DIVIDE"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_MULTIPLY, "KP_MULTIPLY"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_MINUS, "KP_MINUS"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_PLUS, "KP_PLUS"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_ENTER, "KP_ENTER"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_KP_EQUALS, "KP_EQUALS"));

	// Arrows + Home/End pad 
	aIn->AddInputSource(new SdlKeyIs(SDLK_UP, "UP"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_DOWN, "DOWN"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RIGHT, "RIGHT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LEFT, "LEFT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_INSERT, "INSERT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_HOME, "HOME"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_END, "END"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_PAGEUP, "PAGEUP"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_PAGEDOWN, "PAGEDOWN"));

	// Function keys 
	aIn->AddInputSource(new SdlKeyIs(SDLK_F1, "F1"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F2, "F2"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F3, "F3"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F4, "F4"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F5, "F5"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F6, "F6"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F7, "F7"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F8, "F8"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F9, "F9"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F10, "F10"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F11, "F11"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F12, "F12"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F13, "F13"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F14, "F14"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_F15, "F15"));

	// Key state modifier keys 
	aIn->AddInputSource(new SdlKeyIs(SDLK_NUMLOCK, "NUMLOCK"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_CAPSLOCK, "CAPSLOCK"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_SCROLLOCK, "SCROLLOCK"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RSHIFT, "RSHIFT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LSHIFT, "LSHIFT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RCTRL, "RCTRL"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LCTRL, "LCTRL"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RALT, "RALT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LALT, "LALT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_RMETA, "RMETA"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_LMETA, "LMETA"));
	// Left "Windows" key 
	aIn->AddInputSource(new SdlKeyIs(SDLK_LSUPER, "LSUPER"));
	// Right "Windows" key 
	aIn->AddInputSource(new SdlKeyIs(SDLK_RSUPER, "RSUPER"));
	// "Alt Gr" key 
	aIn->AddInputSource(new SdlKeyIs(SDLK_MODE, "MODE"));
	// Multi-key compose key 
	aIn->AddInputSource(new SdlKeyIs(SDLK_COMPOSE, "COMPOSE"));

	// Miscellaneous function keys 
	aIn->AddInputSource(new SdlKeyIs(SDLK_HELP, "HELP"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_PRINT, "PRINT"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_SYSREQ, "SYSREQ"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_BREAK, "BREAK"));
	aIn->AddInputSource(new SdlKeyIs(SDLK_MENU, "MENU"));
	// Power Macintosh power key 
	aIn->AddInputSource(new SdlKeyIs(SDLK_POWER, "POWER"));
	// Some european keyboards 
	aIn->AddInputSource(new SdlKeyIs(SDLK_EURO, "EURO"));
	// Atari keyboard has Undo 
	aIn->AddInputSource(new SdlKeyIs(SDLK_UNDO, "UNDO"));

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Add joystick inputsources.
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	int lIJsCount = SDL_NumJoysticks();
	SDL_Joystick *lJs = 0;
	int lIJsAxes;
	int lIJsButtons;
	for (int lI = 0; lI < lIJsCount; ++lI)
	{
		
		lJs = SDL_JoystickOpen(lI);
		
		mVJoySticks.push_back(lJs);

		// Add joystick axes.
		lIJsAxes = SDL_JoystickNumAxes(lJs);
		for (int lIA = 0; lIA < lIJsAxes; ++lIA)
		{
			aIn->AddInputSource(new SdlAxisIs(SDL_JoystickName(SDL_JoystickIndex(lJs)), lI, lIA, this));
		}

		// Add joystick buttons.
		lIJsButtons = SDL_JoystickNumButtons(lJs);
		for (int lIB = 0; lIB < lIJsButtons; ++lIB)
		{
			aIn->AddInputSource(new SdlButtonIs(SDL_JoystickName(SDL_JoystickIndex(lJs)), lI, lIB, this));
		}
	}

}

SdlInputNexusA::~SdlInputNexusA()
{
	for (unsigned int lI = 0; lI < mVJoySticks.size(); ++lI)
	{
		SDL_JoystickClose(mVJoySticks[lI]);
	}
}	

SdlKeyIs::SdlKeyIs(SDLKey aSdlk, const char *aCKeyString)
{
	mSdlk = aSdlk;

	mBWasActive = false;

	if (aCKeyString)
	{
		mSKey = aCKeyString;
	}
	else
	{
		mSKey = "Key ";
		mSKey += mSdlk;
	}
}

bool SdlKeyIs::IsActive(float &aF) const
{
	// Returns true and amount 1.0 for an active key.
	// returns true and .0 for just-deactivated key (key up).
	// return false for inactive key.
	if (sUi8KeyState[mSdlk])
	{
		aF = 1.0;
		mBWasActive = true;
		return true;
	}
	else
	{
		// If the key was active on the last round, send a key event with amount 0
		// to indicate 'key up'.
		if (mBWasActive)
		{
			aF = 0.0;
			mBWasActive = false;
			return true;
		}
		else
			return false;
	}
}

bool SdlButtonIs::IsActive(float &aF) const
{
	if (SDL_JoystickGetButton(mJs, mIJsButton) != 0)
	{
		aF = 1.0;
		mBWasActive = true;
		return true;
	}
	else
	{
		// If the key was active on the last round, send a key event with amount 0
		// to indicate 'key up'.
		if (mBWasActive)
		{
			aF = 0.0;
			mBWasActive = false;
			return true;
		}
		else
			return false;
	}
}
