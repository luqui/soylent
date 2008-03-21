#include "InputNexus.h"
#include "Exception.h"
using namespace bburdette;

InputNexus::~InputNexus()
{
	// delete all the things.
	{
		CsListType::iterator lIter = mLCsList.begin();
		while (lIter != mLCsList.end())
		{
			delete (*lIter);
			++lIter;
		}
	}

	{
		IsListType::iterator lIter = mLIsList.begin();
		while (lIter != mLIsList.end())
		{
			delete (*lIter);
			++lIter;
		}
	}
}

// This sets up the ptrs to the actual InputSources for each CommandSpec, and 
// puts commandspecs into the mSCsActive set.
void InputNexus::Init()
{

	// Clear any command-inputsource associations.
	mSCsActive.clear();
	CsListType::iterator lCltIter = mLCsList.begin();
	while (lCltIter != mLCsList.end())
	{
		(*lCltIter)->mLIsList.clear();
		++lCltIter;
	}

	// Init InputSources.
	for (IsListType::iterator lIltIter = mLIsList.begin(); lIltIter != mLIsList.end(); ++lIltIter)
	{
		(*lIltIter)->Init();
	}


	Controllable::CommandSpec *lCs;
	InputSource *lIs;
	IsCsMapType::const_iterator lIter = mMIsCsAssociations.begin();
	while (lIter != mMIsCsAssociations.end())
	{
		lCs = FindCs(lIter->first);
		lIs = FindIs(lIter->second);
		if (lCs && lIs)
		{
			// Add the InputSource to the CommandSpec.
			lCs->AddIs(lIs);

			// Add the commandspec to the active list.
			mSCsActive.insert(lCs);
		}
		++lIter;
	}

}

InputSource* InputNexus::FindIs(const string &aSId)
{
	IsListType::iterator lIter = mLIsList.begin();
	while (lIter != mLIsList.end())
	{
		if ((*lIter)->KeyString() == aSId.c_str())
			return *lIter;
		++lIter;
	}
	return 0;
}

Controllable::CommandSpec* InputNexus::FindCs(const string &aSName)
{
	CsListType::iterator lIter = mLCsList.begin();
	while (lIter != mLCsList.end())
	{
		if ((*lIter)->mSName == aSName)
			return *lIter;
		++lIter;
	}
	return 0;
}

unsigned char InputNexus::AddCommandSpec(Controllable::CommandSpec *aCs)
{
	// First, check for a duplicate command.
	Controllable::CommandSpec *lCs = FindCs(aCs->mSName);
	if (lCs)
	{
		delete aCs;
		return lCs->mUcId;
	}

	aCs->mUcId = mIIdCounter;
	if (++mIIdCounter > 255)
		throw MessageException("More than 255 commands!");

	mLCsList.push_back(aCs);
	return aCs->mUcId;
}

void InputNexus::ExecuteCommands(Controllable* aC)
{
	CsSetType::iterator lIter = mSCsActive.begin();
	while (lIter != mSCsActive.end())
	{
		(*lIter)->ExecuteCommand(aC);
		++lIter;
	}
}

// InputNexus defns

void InputNexus::ToOs(ostream &aOs) const
{
	NVThing lNvt;

	ToNvt(lNvt);

	aOs << lNvt;
}
void InputNexus::FromIs(istream &aIs)
{
	NVThing lNvt;

	aIs >> lNvt;

	NVThing *lNvtSection;

	bool lB = lNvt.Find("AvailableCommands", lNvtSection);

	FromNvt(lNvt);
}

void InputNexus::ToNvt(NVThing &aNvt) const
{
	// --------------------------------------------------------------
	// Write out available commands.
	// --------------------------------------------------------------
	CsListType::const_iterator lIter = mLCsList.begin();
	int lICount = 0;
	NVThing *lNvtSection = new NVThing;
	while (lIter != mLCsList.end())
	{
		NVThing *lNvt = new NVThing;
		(*lIter)->ToNvt(*lNvt);
		lNvtSection->Add("Command", lICount, (*lIter)->mSName.c_str());
		++lIter;
		++lICount;
	}
	lNvtSection->Add("Count", lICount);
	aNvt.Add("AvailableCommands", lNvtSection);

	// --------------------------------------------------------------
	// Write out available InputSources.
	// --------------------------------------------------------------
	{
		IsListType::const_iterator lIter = mLIsList.begin();
		int lICount = 0;
		lNvtSection = new NVThing;
		while (lIter != mLIsList.end())
		{
			lNvtSection->Add("InputSource", lICount, (*lIter)->KeyString().c_str());
			++lIter;
			++lICount;
		}
		lNvtSection->Add("Count", lICount);
		aNvt.Add("InputSources", lNvtSection);
	}

	// --------------------------------------------------------------
	// Write out command-InputSource associations.
	// --------------------------------------------------------------
	lNvtSection = new NVThing;
	IsCsMapType::const_iterator lCIter = mMIsCsAssociations.begin();
	lICount = 0;
	while (lCIter != mMIsCsAssociations.end())
	{
		lNvtSection->Add("Command", lICount, lCIter->first.c_str());
		lNvtSection->Add("InputSource", lICount, lCIter->second.c_str());
		++lCIter;
		++lICount;
	}

	lNvtSection->Add("Count", lICount);
	aNvt.Add("ActiveCommands", lNvtSection);
}

void InputNexus::FromNvt(const NVThing &aNvt)
{
	// --------------------------------------------------------------
	// Read in the associations section, ignoring the 
	// "available commands" and "available inputsources" sections.
	// --------------------------------------------------------------
	mMIsCsAssociations.clear();
	NVThing *lNvtSection;

	if (aNvt.Find("ActiveCommands", lNvtSection))
	{
		int lICount;
		lNvtSection->Find("Count", lICount);

		for (int lI = 0; lI < lICount; ++lI)
		{
			const char *lCName;
			const char *lCInputSource;
			if (lNvtSection->Find("Command", lI, lCName) && 
				lNvtSection->Find("InputSource", lI, lCInputSource))
			{
				mMIsCsAssociations.insert(IsCsMapType::value_type(lCName, lCInputSource));
			}
		}
	}
}

