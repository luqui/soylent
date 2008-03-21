#ifndef NVThing_h
#define NVThing_h

#include "Port.h"
#include "Archivable.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// 'Name-Value' thing.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Heirarchical Name-Value pairs, ie you can assocate strings with names within an 
// NVThing, and you can take an NVThing and put it inside another NVThing.  
// Functions are provided to storing other types.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Important Regarding streams!
// In microsoft vc++ at least you must read in streams in binary mode.  Maybe a bug, who 
// knows, but that's what you gotta do.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
class NVArchivable;

class NVThing : public Archivable
{
private:
	class NVData
	{
	public:
		string* mS;
		NVThing* mNvt;
		NVData(const char* aC)
		{
			mS = new string(aC);
			mNvt = 0;
		}
		NVData(NVThing *aNvt)
		{
			mS = 0;
			mNvt = aNvt;
		}

		~NVData()
		{
			if (mS)
				delete mS;
			if (mNvt)
				delete mNvt;
		}
	};
	typedef map<string, NVData*> NVMapType;
	NVMapType mMValues;
	mutable int mILevel;
	mutable string mSWk;

	inline const char* IndexName(const char *aCName, int aIIndex) const
	{
		mSWk = aCName;
		char lC[30];
        sprintf(lC, "%d", aIIndex);
		mSWk += lC;
		return mSWk.c_str();
	}
public:
	NVThing(int aILevel = 0) { mILevel = aILevel; }
	virtual ~NVThing();

	NVThing& operator=(const NVThing &aNvt);

	void Clear();

	inline void Add(const char *aCName, const char* aCValue);
	inline bool Find(const char *aCName, const char *&aCValue) const;

	// - When you Add an NVThing, the parent takes over ownership
	// of that object.  Do not then add the NVThing to another
	// NVThing, this will result in a double delete later on.
	// - Also you are not allowed to read from a stream into an 
	// NVThing you just added, only a 'base level' NVThing.
	inline void Add(const char *aCName, NVThing *aNvt);
	inline bool Find(const char *aCName, NVThing *&aNvt) const;

	void Add(const char *aCName, const NVArchivable &aNva);
	bool Find(const char *aCName, NVArchivable &aNva) const;

	// Add for various types.  These all end up calling Add for string.
	inline void Add(const char *aCName, const int64_t &aI64);
	inline bool Find(const char *aCName, int64_t &aI64) const;
	inline void Add(const char *aCName, const int &aI);
	inline bool Find(const char *aCName, int &aI) const;
	inline void Add(const char *aCName, const unsigned int &aUi);
	inline bool Find(const char *aCName, unsigned int &aUi) const;
	inline void Add(const char *aCName, const short &aI);
	inline bool Find(const char *aCName, short &aI) const;
	inline void Add(const char *aCName, const double &aD);
	inline bool Find(const char *aCName, double &aD) const;
	inline void Add(const char *aCName, const float &aD);
	inline bool Find(const char *aCName, float &aD) const;
	inline void Add(const char *aCName, const char aC);
	inline bool Find(const char *aCName, char &aC) const;
	inline void Add(const char *aCName, const bool aB);
	inline bool Find(const char *aCName, bool &aB) const;

	inline bool Delete(const char *aCName);
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Index Versions of the ftns.
	inline void Add(const char *aCName, int aIIndex, const char* aCValue);
	inline bool Find(const char *aCName, int aIIndex, const char *&aCValue) const;
	inline void Add(const char *aCName, int aIIndex, NVThing *aNvt);
	inline bool Find(const char *aCName, int aIIndex, NVThing *&aNvt) const;
	inline void Add(const char *aCName, int aIIndex, const int64_t &aI64);
	inline bool Find(const char *aCName, int aIIndex, int64_t &aI64) const;
	inline void Add(const char *aCName, int aIIndex, const int &aI);
	inline bool Find(const char *aCName, int aIIndex, int &aI) const;
	inline void Add(const char *aCName, int aIIndex, const unsigned int &aI);
	inline bool Find(const char *aCName, int aIIndex, unsigned int &aI) const;
	inline void Add(const char *aCName, int aIIndex, const short &aI);
	inline bool Find(const char *aCName, int aIIndex, short &aI) const;
	inline void Add(const char *aCName, int aIIndex, const double &aD);
	inline bool Find(const char *aCName, int aIIndex, double &aD) const;
	inline void Add(const char *aCName, int aIIndex, const char aC);
	inline bool Find(const char *aCName, int aIIndex, char &aC) const;
	inline void Add(const char *aCName, int aIIndex, const bool aB);
	inline bool Find(const char *aCName, int aIIndex, bool &aB) const;
	inline bool Delete(const char *aCName, int aIIndex);
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void ToOs(ostream &aOs) const;
	void FromIs(istream &aIs);

private:

	static const char *sCVersion;

	void ToOs_Internal(ostream &aOs) const;
	void FromIs_Internal(istream &aIs);

	void FromIs_Old(istream &aIs);

	//virtual void ToXml(ostream &aOs) const;

};

// ---------------------------------------------------------
// NVArchivable ancestor.  Make any object Add/Find able with
// this ancestor.
// ---------------------------------------------------------

class NVArchivable
{
public:
	virtual void ToNvt(NVThing &aNvt) const = 0;
	virtual void FromNvt(const NVThing &aNvt) = 0;
};

/*
void NewObj::ToNvt(NVThing &aNvt) const
{
}
void NewObj::FromNvt(const NVThing &aNvt)
{
}
*/

// ---------------------------------------------------------
// Implementations of the inline ftns.
// ---------------------------------------------------------

inline void NVThing::Add(const char *aCName, const char* aCValue)
{
	mMValues.insert(NVMapType::value_type(aCName, new NVData(aCValue)));
}

inline bool NVThing::Find(const char *aCName, const char* &aCValue) const
{
	NVMapType::const_iterator lIter = mMValues.find(aCName);
	if (lIter != mMValues.end() && lIter->second->mS)
	{
		aCValue = lIter->second->mS->c_str();
		return true;
	}
	else
		return false;		// not found or not a string.
}

inline void NVThing::Add(const char *aCName, NVThing *aNvt)
{
	//aNvt->mILevel = mILevel + 1;		// do this on output.
	mMValues.insert(NVMapType::value_type(aCName, new NVData(aNvt)));
}

inline bool NVThing::Find(const char *aCName, NVThing *&aNvt) const
{
	NVMapType::const_iterator lIter = mMValues.find(aCName);
	if (lIter != mMValues.end() && lIter->second->mNvt)
	{
		aNvt = lIter->second->mNvt;
		return true;
	}
	else
		return false;		// not found or not a string.
}

inline void NVThing::Add(const char *aCName, const int &aI)
{
	char lC[100];
    sprintf(lC, "%d", aI);
	Add(aCName, lC);
}
inline bool NVThing::Find(const char *aCName, int &aI) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		aI = atoi(lC);
		return true;
	}
	else
		return false;
}

inline void NVThing::Add(const char *aCName, const short &aS)
{
	Add(aCName, (int)aS);
}
inline bool NVThing::Find(const char *aCName, short &aS) const
{
	int lI;
	if (Find(aCName, lI))
	{
		aS = (short)lI;
		return true;
	}
	else
		return false;
}

inline void NVThing::Add(const char *aCName, const unsigned int &aUi)
{
	char lC[100];
    sprintf(lC, "%d", aUi);
	Add(aCName, lC);
}
inline bool NVThing::Find(const char *aCName, unsigned int &aUi) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		aUi = atoi(lC);
		return true;
	}
	else
		return false;
}

inline void NVThing::Add(const char *aCName, const char aC)
{
	char lC[2];
	lC[0] = aC;
	lC[1] = '\0';
	Add(aCName, lC);
}

inline bool NVThing::Find(const char *aCName, char &aC) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		aC = *lC;
		return true;
	}
	else
		return false;
}

void NVThing::Add(const char *aCName, const double &aD)
{
	char lC[30];
	sprintf(lC, "%.16g", aD);
    Add(aCName, lC);
}
bool NVThing::Find(const char *aCName, double &aD) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		aD = atof(lC);
		return true;
	}
	else
		return false;
}

void NVThing::Add(const char *aCName, const float &aF)
{
	char lC[30];
	sprintf(lC, "%.16g", aF);
    Add(aCName, lC);
}
bool NVThing::Find(const char *aCName, float &aF) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		aF = static_cast<float>(atof(lC));
		return true;
	}
	else
		return false;
}

inline void NVThing::Add(const char *aCName, const int64_t &aI64)
{
	char lC[100];
    sprintf(lC, "%lld", aI64);
	Add(aCName, lC);
}

inline bool NVThing::Find(const char *aCName, int64_t &aI64) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		aI64 = atoll(lC);
		return true;
	}
	else
		return false;
}

inline void NVThing::Add(const char *aCName, const bool aB)
{
	if (aB)
		Add(aCName, "true");
	else
		Add(aCName, "false");
}

inline bool NVThing::Find(const char *aCName, bool &aB) const
{
	const char *lC;
	if (Find(aCName, lC))
	{
		if (strnicmp(lC, "true", 4) == 0)
			aB = true;
		else
			aB = false;

		return true;
	}
	else
		return false;
}



bool NVThing::Delete(const char *aCName)
{
	NVMapType::iterator lIter = mMValues.find(aCName);
	if (lIter != mMValues.end())
	{
		delete lIter->second;
		mMValues.erase(lIter);
		return true;
	}
	else
		return false;		// not found.
}

inline void NVThing::Add(const char *aCName, int aIIndex, const char* aCValue)
{
	Add(IndexName(aCName, aIIndex), aCValue);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, const char *&aCValue) const
{
	return Find(IndexName(aCName, aIIndex), aCValue);
}

inline void NVThing::Add(const char *aCName, int aIIndex, NVThing *aNvt)
{
	Add(IndexName(aCName, aIIndex), aNvt);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, NVThing *&aNvt) const
{
	return Find(IndexName(aCName, aIIndex), aNvt);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const int64_t &aI64)
{
	Add(IndexName(aCName, aIIndex), aI64);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, int64_t &aI64) const
{
	return Find(IndexName(aCName, aIIndex), aI64);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const int &aI)
{
	Add(IndexName(aCName, aIIndex), aI);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, int &aI) const
{
	return Find(IndexName(aCName, aIIndex), aI);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const unsigned int &aUi)
{
	Add(IndexName(aCName, aIIndex), aUi);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, unsigned int &aUi) const
{
	return Find(IndexName(aCName, aIIndex), aUi);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const short &aS)
{
	Add(IndexName(aCName, aIIndex), aS);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, short &aS) const
{
	return Find(IndexName(aCName, aIIndex), aS);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const char aC)
{
	Add(IndexName(aCName, aIIndex), aC);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, char &aC) const
{
	return Find(IndexName(aCName, aIIndex), aC);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const bool aB)
{
	Add(IndexName(aCName, aIIndex), aB);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, bool &aB) const
{
	return Find(IndexName(aCName, aIIndex), aB);
}

inline void NVThing::Add(const char *aCName, int aIIndex, const double &aD)
{
	Add(IndexName(aCName, aIIndex), aD);
}
inline bool NVThing::Find(const char *aCName, int aIIndex, double &aD) const
{
	return Find(IndexName(aCName, aIIndex), aD);
}

inline bool NVThing::Delete(const char *aCName, int aIIndex)
{
	return Delete(IndexName(aCName, aIIndex));
}

// Rules for NVThing files.
// NVThing files consist of alternating name and value lines.
// Indentions with the tab character indicate 'subobjects', embedded NVThings.

/*

Name
Ben
Date
11/06/68
Possessions
	Name0
	Guitar
	Name1
	Computer
	Name2
	Rug
Location
Columbus

*/



#endif
