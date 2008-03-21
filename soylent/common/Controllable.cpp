#include "Controllable.h"

// This should pass commands to the Controllable
// via Controllable::ExecuteCommand()
void Controllable::CommandSpec::ExecuteCommand(Controllable *aC)
{
	Command lC;
	lC.mUcId = mUcId;
	list<InputSource*>::iterator lIter = mLIsList.begin();
	while (lIter != mLIsList.end())
	{
		if ((*lIter)->IsActive(lC.mFAmt))
		{
			lC.mBRelative = (*lIter)->IsRelative();
			// If a min time is specified, don't execute the command until 
			// that time is exceeded.
			if (mCtMinTime)
			{
				clock_t lCt = clock();
				if (lCt - mCtLast > mCtMinTime)
					mCtLast = lCt;
				else
					return;
			}

			aC->ExecuteCommand(&lC);
			break;		// stop after the first active InputSource.
		}
		++lIter;
	}
}

void Controllable::CommandSpec::ToNvt(NVThing &aNvt) const
{
	aNvt.Add("Name", mSName.c_str());
	aNvt.Add("Id", mUcId);

	list<string>::const_iterator lIter = mLIsKeys.begin();
	int lI = 0;
	while (lIter != mLIsKeys.end())
	{
		aNvt.Add("InputSource", lI, (*lIter).c_str());
		++lIter;
		++lI;
	}
}

void Controllable::CommandSpec::FromNvt(const NVThing &aNvt)
{
	const char* lC;
	aNvt.Find("Name", lC);
	mSName = lC;
	char lCWk;
	aNvt.Find("Id", lCWk);
	mUcId = (unsigned char)lCWk;

	int lI = 0;
	while (aNvt.Find("InputSource", lI, lC))
	{
		mLIsKeys.push_back(lC);
		++lI;
	}
}
