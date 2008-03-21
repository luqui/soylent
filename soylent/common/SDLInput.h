#ifndef SDLInput_h
#define SDLInput_h

// Input sources for SDL.

#include "Port.h"
#include "InputNexus.h"
#include <SDL.h>
#include <vector>
using namespace std;

// InputNexus layer for SDL input.
// T should be an InputNexus or descendant.

class SdlKeyIs;

class SdlInputNexusA
{
	vector<SDL_Joystick*> mVJoySticks;
protected:
	void DiscoverInputSourcesA(InputNexus *aIn);
public:
	virtual ~SdlInputNexusA();
	SDL_Joystick* GetJoyStick(int aIIndex) { return mVJoySticks[aIIndex]; }
};

class SdlKeyIs : public InputSource
{
public:
	static Uint8 *sUi8KeyState;
	static int *sIKeyCount;
	SDLKey mSdlk;
	string mSKey;
	mutable bool mBWasActive;

	SdlKeyIs(SDLKey aSdlk, const char *aCKeyDescription = 0);
    virtual ~SdlKeyIs() { }

	virtual bool Init() { return true; }

	virtual bool IsActive(float &aF) const;
	virtual bool IsRelative() const { return true; }
	virtual const string& KeyString() const
	{
		return mSKey;
	}
};

template <class T> 
class TSdlInputNexus : public T, public SdlInputNexusA
{
public:
	virtual void DiscoverInputSources()
	{
		T::DiscoverInputSources();
		SdlInputNexusA::DiscoverInputSourcesA(this);
	}

	virtual void ExecuteCommands(Controllable* aC)
	{
		SDL_PumpEvents();

		SdlKeyIs::sUi8KeyState = SDL_GetKeyState(SdlKeyIs::sIKeyCount);

		T::ExecuteCommands(aC);
	}
};

class SdlAxisIs : public InputSource
{
private:
	string mSKey;
	SDL_Joystick *mJs;
	int mIJsIndex;
	int mIJsAxis;
	SdlInputNexusA *mSinaParent;
//	string mSName;
public:

	SdlAxisIs(const char *aCName, int aIJsIndex, int aIJsAxis, SdlInputNexusA *aSinaParent)
		:mIJsIndex(aIJsIndex), mIJsAxis(aIJsAxis), mSinaParent(aSinaParent)
	{
		char lC[20];
		mSKey = aCName;
        sprintf(lC, " %d", mIJsIndex + 1);
		mSKey += lC;
		mSKey += " Axis ";
        sprintf(lC, "%d", mIJsAxis + 1);
		mSKey += lC;
	}

    virtual ~SdlAxisIs() { }

	virtual bool Init() 
	{ 
		mJs = mSinaParent->GetJoyStick(mIJsIndex);

		return mJs != 0;
	}

	virtual bool IsActive(float &aF) const
	{
		aF = SDL_JoystickGetAxis(mJs, mIJsAxis);
		aF /= 32767.0;		// ranges between 0 and 1.
		return true;
	}
	virtual bool IsRelative() const { return false; }
	virtual const string& KeyString() const
	{
		return mSKey;
	}
};

class SdlButtonIs : public InputSource
{
public:

	SdlButtonIs(const char *aCName, int aIJsIndex, int aIJsButton, SdlInputNexusA *aSinaParent)
		:mIJsIndex(aIJsIndex), mIJsButton(aIJsButton), mSinaParent(aSinaParent), mBWasActive(false)
	{
		char lC[20];
		mSKey = aCName;
        sprintf(lC, " %d", mIJsIndex + 1);
		mSKey += lC;
		mSKey += " Button ";
        sprintf(lC, "%d", mIJsButton + 1);
		mSKey += lC;
	}

    virtual ~SdlButtonIs() { }

	virtual bool Init() 
	{ 
		mJs = mSinaParent->GetJoyStick(mIJsIndex);
		return mJs != 0; 
	}

	virtual bool IsActive(float &aF) const;
	virtual bool IsRelative() const { return true; }

	virtual const string& KeyString() const
	{
		return mSKey;
	}
private:
	SDL_Joystick *mJs;
	int mIJsIndex;
	int mIJsButton;
	string mSKey;
	SdlInputNexusA *mSinaParent;
	mutable bool mBWasActive;

};

#endif 
