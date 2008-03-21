#ifndef ManyMouseInput_h
#define ManyMouseInput_h

// Input sources for SDL.

#include "InputNexus.h"
#include "manymouse.h"
using namespace std;

// InputNexus layer for SDL input.
// T should be an InputNexus or descendant.

//    MANYMOUSE_EVENT_ABSMOTION = 0,
//    MANYMOUSE_EVENT_RELMOTION,
//    MANYMOUSE_EVENT_BUTTON,
//    MANYMOUSE_EVENT_SCROLL,
//    MANYMOUSE_EVENT_DISCONNECT,
//    MANYMOUSE_EVENT_MAX

class ManyMouseIs : public InputSource
{
	friend class ManyMouseInputNexusA;
public:
	ManyMouseIs(const char *aCName, ManyMouseEventType aType, unsigned int aUiDevice, unsigned int aUiItem);
    virtual ~ManyMouseIs() { }

	bool Init() { return true; }

	virtual void OnPollStart() = 0;

	void SetActive(bool aB, const float &aF)
	{
		mBActive = aB;
		mFAmt = aF;
	}

	virtual bool IsRelative() const { return true; }

	virtual const string& KeyString() const
	{
		return mSKey;
	}

	const bool GetActive() const { return mBActive; }
	const float& GetAmt() const { return mFAmt; }

private:
	ManyMouseEventType mType;
	unsigned int mUiDevice;
	unsigned int mUiItem;
	string mSKey;
	bool mBActive;
	float mFAmt;
};

class MmAxisIs : public ManyMouseIs
{
	friend class ManyMouseInputNexusA;
public:
	MmAxisIs(const char *aCName, ManyMouseEventType aType, unsigned int aUiDevice, unsigned int aUiItem)
	:ManyMouseIs(aCName, aType, aUiDevice, aUiItem)
	{}

	bool Init() { return true; }

	virtual void OnPollStart()
	{
		SetActive(false, .0);
	}

	virtual bool IsActive(float &aF) const
	{
		aF = GetAmt();
		return GetActive();
	}
};

class MmButtonIs : public ManyMouseIs
{
	friend class ManyMouseInputNexusA;
public:
	MmButtonIs(const char *aCName, ManyMouseEventType aType, unsigned int aUiDevice, unsigned int aUiItem)
	:ManyMouseIs(aCName, aType, aUiDevice, aUiItem), mBWasActive(false)
	{}

	bool Init() { return true; }

	virtual void OnPollStart() {}

	virtual bool IsActive(float &aF) const;
private:
	mutable bool mBWasActive;
};

class ManyMouseInputNexusA
{
public:
	ManyMouseInputNexusA();
	virtual ~ManyMouseInputNexusA();
protected:
	void DiscoverInputSourcesA(InputNexus *aIn);
	void PollDevices();
	int mIMouseCount;
	typedef map<unsigned int, ManyMouseIs*> MmiMap;
	MmiMap mMmIsMap;
	inline unsigned int CalcMmKey(unsigned int aUiType, unsigned int aUiDevice, unsigned int aUiItem)
	{
		unsigned int lUiReturn(aUiType);
		lUiReturn <<= 8;
		lUiReturn |= aUiDevice;
		lUiReturn <<= 8;
		lUiReturn |= aUiItem;

		return lUiReturn;
	}
};

template <class T> 
class TManyMouseInputNexus : public T, public ManyMouseInputNexusA
{
public:
	virtual void DiscoverInputSources()
	{
		T::DiscoverInputSources();
		ManyMouseInputNexusA::DiscoverInputSourcesA(this);
	}

	virtual void ExecuteCommands(Controllable* aC)
	{
		ManyMouseInputNexusA::PollDevices();
		T::ExecuteCommands(aC);
	}
};

#endif
