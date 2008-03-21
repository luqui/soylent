#include "FlockController.h"

// ---------------------------------------------------------------------------------
// FlockController
// ---------------------------------------------------------------------------------

void FlockController::AddControllable(Controllable *aC)
{
	mLControllables.push_front(aC);
}

void FlockController::AddCommandSpecs(CommandSpecContainer *aCsc)
{
	if (mLControllables.empty())
		return;

	// Add command specs for all objects.
	list<Controllable*>::iterator lIter = mLControllables.begin();
	for (;lIter != mLControllables.end(); ++lIter)
	{
		(*lIter)->AddCommandSpecs(aCsc);
	}
}

void FlockController::ExecuteCommand (Command *aC)
{
	for (list<Controllable*>::iterator lIter = mLControllables.begin(); 
		lIter != mLControllables.end(); ++lIter)
	{
		(*lIter)->ExecuteCommand(aC);
	}
}

void FlockController::RemoveControllable(Controllable *aC) 
{ 
	for (list<Controllable*>::iterator lIter = mLControllables.begin(); 
		lIter != mLControllables.end(); ++lIter)
	{
		if (*lIter == aC)
		{
			mLControllables.erase(lIter);
			return;
		}

		(*lIter)->RemoveControllable(aC);
	}
}
// ---------------------------------------------------------------------------------
// ControlSwitcher
// ---------------------------------------------------------------------------------

ControlSwitcher::ControlSwitcher()
{
	mICurrent = 0;
	mCtMinInterval = 500;
	mCtLast = 0;
}

void ControlSwitcher::AddControllable(Controllable *aC)
{
	mVControllables.push_back(aC);
}

void ControlSwitcher::AddCommandSpecs(CommandSpecContainer *aCsc)
{
	mUcSwitch = aCsc->AddCommandSpec(new Controllable::CommandSpec("Switch Control Object"));

	if (mVControllables.empty())
		return;

	// Add command specs for all objects, this allows the objects to get the command ids too.
	vector<Controllable*>::iterator lIter = mVControllables.begin();
	for (;lIter != mVControllables.end(); ++lIter)
	{
		(*lIter)->AddCommandSpecs(aCsc);
	}
}

void ControlSwitcher::RemoveControllable(Controllable *aC) 
{ 
	for (vector<Controllable*>::iterator lIter = mVControllables.begin(); 
		lIter != mVControllables.end(); ++lIter)
	{
		if (*lIter == aC)
		{
			mVControllables.erase(lIter);
			return;
		}

		(*lIter)->RemoveControllable(aC);
	}
}

void ControlSwitcher::ExecuteCommand(Command *aC)
{
	if (mVControllables.empty())
		return;

	if (aC->mUcId == mUcSwitch)
	{
		clock_t lCt = clock();
		if (lCt - mCtLast > mCtMinInterval)
		{
			mCtLast = lCt;
		}
		else
			return;

		++mICurrent;
		if (mICurrent == mVControllables.size())
			mICurrent = 0;
	}
	else
		mVControllables[mICurrent]->ExecuteCommand(aC);
}

// ---------------------------------------------------------------------------------
// MultiPlayer
// ---------------------------------------------------------------------------------
void MultiPlayer::AddControllable(Controllable *aC, const char *aCPrefix)
{
	mVControllables.push_back(aC);
	mVPrefixes.push_back(aCPrefix);
}

void MultiPlayer::AddCommandSpecs(CommandSpecContainer *aCsc)
{
	if (mVControllables.empty())
		return;

	PrefixCsc lPc(aCsc, "");
	// Add command specs for all objects, this allows the objects to get the command ids too.
	for (unsigned int lI = 0; lI < mVControllables.size(); ++lI)
	{
		lPc.mSPrefix = mVPrefixes[lI];
		mVControllables[lI]->AddCommandSpecs(&lPc);
	}
}

void MultiPlayer::ExecuteCommand(Command *aC)
{
	if (mVControllables.empty())
		return;

	// for now, send each command to all objects.  They will discard the ones that 
	// don't apply.
	// Add command specs for all objects, this allows the objects to get the command ids too.
	for (unsigned int lI = 0; lI < mVControllables.size(); ++lI)
	{
		mVControllables[lI]->ExecuteCommand(aC);
	}
}

void MultiPlayer::RemoveControllable(Controllable *aC) 
{ 
	for (vector<Controllable*>::iterator lIter = mVControllables.begin(); 
		lIter != mVControllables.end(); ++lIter)
	{
		if ((*lIter) == aC)
		{
			mVControllables.erase(lIter);
			return;
		}

		(*lIter)->RemoveControllable(aC);
	}
}

MultiPlayer::PrefixCsc::PrefixCsc(CommandSpecContainer *aCsc, const char *aCPrefix)
{
	mSPrefix = aCPrefix;
	mCsc = aCsc;
}

unsigned char MultiPlayer::PrefixCsc::AddCommandSpec(Controllable::CommandSpec *aCs)
{
	// Append the prefix to all commands before passing them to the internal CSC.
	aCs->mSName = mSPrefix + aCs->mSName;

	return mCsc->AddCommandSpec(aCs);
}

