#include "ManyMouseInput.h"
#include "Exception.h"
using namespace bburdette;
#include <sstream>

ManyMouseInputNexusA::ManyMouseInputNexusA()
{
	mIMouseCount = ManyMouse_Init();
}

ManyMouseInputNexusA::~ManyMouseInputNexusA()
{
	ManyMouse_Quit();
}

void ManyMouseInputNexusA::DiscoverInputSourcesA(InputNexus *aIn)
{
	for (int lI = 0; lI < mIMouseCount; ++lI)
	{
		// X and Y axis.
		ManyMouseIs *lMmi;

        std::ostringstream lOss;
		lOss << "Mouse " << lI << '\0'; 

		lMmi = new MmAxisIs(lOss.str().c_str(), MANYMOUSE_EVENT_RELMOTION, lI, 0);
		mMmIsMap.insert(MmiMap::value_type(CalcMmKey(lMmi->mType, lMmi->mUiDevice, lMmi->mUiItem), lMmi));
		aIn->AddInputSource(lMmi);

		lMmi = new MmAxisIs(lOss.str().c_str(), MANYMOUSE_EVENT_RELMOTION, lI, 1);
		mMmIsMap.insert(MmiMap::value_type(CalcMmKey(lMmi->mType, lMmi->mUiDevice, lMmi->mUiItem), lMmi));
		aIn->AddInputSource(lMmi);

		lMmi = new MmButtonIs(lOss.str().c_str(), MANYMOUSE_EVENT_BUTTON, lI, 0);
		mMmIsMap.insert(MmiMap::value_type(CalcMmKey(lMmi->mType, lMmi->mUiDevice, lMmi->mUiItem), lMmi));
		aIn->AddInputSource(lMmi);

		lMmi = new MmButtonIs(lOss.str().c_str(), MANYMOUSE_EVENT_BUTTON, lI, 1);
		mMmIsMap.insert(MmiMap::value_type(CalcMmKey(lMmi->mType, lMmi->mUiDevice, lMmi->mUiItem), lMmi));
		aIn->AddInputSource(lMmi);

		// 2 Buttons for now, no way to know the button count through manymouse.
	}
}

//typedef enum
//{
//    MANYMOUSE_EVENT_ABSMOTION = 0,
//    MANYMOUSE_EVENT_RELMOTION,
//    MANYMOUSE_EVENT_BUTTON,
//    MANYMOUSE_EVENT_SCROLL,
//    MANYMOUSE_EVENT_DISCONNECT,
//    MANYMOUSE_EVENT_MAX
//} ManyMouseEventType;
//
//typedef struct
//{
//    ManyMouseEventType type;
//    unsigned int device;
//    unsigned int item;
//    int value;
//    int minval;
//    int maxval;
//} ManyMouseEvent;

void ManyMouseInputNexusA::PollDevices()
{
	// Set all controls to inactive to begin with.
	MmiMap::iterator lIter = mMmIsMap.begin();
	while (lIter != mMmIsMap.end())
	{
		lIter->second->OnPollStart();
		++lIter;
	}

	// Activate the ones that got events.
	ManyMouseEvent lMme;
	while (ManyMouse_PollEvent(&lMme))
	{
		lIter = mMmIsMap.find(CalcMmKey(lMme.type, lMme.device, lMme.item));
		if (lIter != mMmIsMap.end())
		{
			switch (lMme.type)
			{
			case MANYMOUSE_EVENT_RELMOTION:
				{
					float lF(lMme.value);
					lF /= 50.0;
					if (lF > 1.0)
						lF = 1.0;
					else if (lF < -1.0)
						lF = -1.0;

					lIter->second->SetActive(true, lF);
					break;
				}
			case MANYMOUSE_EVENT_ABSMOTION:
				{
					float lF(lMme.value - lMme.minval);
					lF /= lMme.maxval;
					lIter->second->SetActive(true, lF);
					break;
				}
			case MANYMOUSE_EVENT_BUTTON:
				{
					lIter->second->SetActive(lMme.value == 1, lMme.value);
					break;
				}
			default:
				// ignore other events.
				;
			}
		}
	}
}

ManyMouseIs::ManyMouseIs(const char *aCName, ManyMouseEventType aType, unsigned int aUiDevice, unsigned int aUiItem)
	:mType(aType), mUiDevice(aUiDevice), mUiItem(aUiItem), mBActive(false), mFAmt(.0)
{
	char lC[20];
	mSKey = aCName;
	switch (mType)
	{
	case MANYMOUSE_EVENT_RELMOTION:
		if (aUiItem == 0)
			mSKey += " X Axis";
		else
			mSKey += " Y Axis";
		break;
	case MANYMOUSE_EVENT_BUTTON:
		mSKey += " Button ";
        sprintf(lC, "%d", aUiItem + 1);
		mSKey += lC;
		break;
	default:
		throw MessageException("Invalid ManyMouse type!");
	}
}


bool MmButtonIs::IsActive(float &aF) const
{
	// Returns true and amount 1.0 for an active key.
	// returns true and .0 for just-deactivated key (key up).
	// return false for inactive key.
	if (GetActive())
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
