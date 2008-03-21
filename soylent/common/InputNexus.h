#ifndef InputNexus_h
#define InputNexus_h

#include "Port.h"
#include "Controllable.h"
#include "Archivable.h"
#include "NVThing.h"
#include <string>
#include <list>
#include <set>

using namespace std;

// Relevant Objects used here:

// InputSource:  represents a joystick axis, keyboard key, mouse axis, etc.
// (in Controllable.h)

// Controllable::CommandSpec:  
//				This is a description of a command that an object supports, together with the ID of the command.

// Controllable::Command:  
//				This is a small object that contains a command ID and an amount.  The amount may or may not be used.

// InputNexus:   This object associates InputSources and CommandSpecs.  In its ExecuteCommands() method, it 
//				1) scans for active InputSources 
//				2) locates Controllable::CommandSpecs object associated with these.
//				3) creates Controllable::Command objects from the CommandSpecs.
//				4) passes these Command objects to the Controllable object that is the
//				   argument to ExecuteCommands().  

// With the requirement of being able to load an InputNexus config for controls that we 
// don't have on the current machine, I'm going to have a function to read in the associations, 
// and another to actually locate and associate the objects. 

// The InputNexus lists all CommandSpecs and InputSources, and associates them together.
class InputNexus : public NVArchivable, public Archivable, public CommandSpecContainer
{
public:
private:
	typedef list<Controllable::CommandSpec*> CsListType;
	CsListType mLCsList;
	typedef list<InputSource*> IsListType;
	IsListType mLIsList;

	// Each new CommandSpec gets a unique id.  This is the counter for that.
	int mIIdCounter;

	// Map of associations between InputSource key strings and CommandSpec names.
	// These are the associations read in from the file; not all key strings or 
	// CommandSpecs may be valid in the current game.
	typedef multimap<string, string> IsCsMapType;
	IsCsMapType mMIsCsAssociations;

	InputSource* FindIs(const string &aSId);
	Controllable::CommandSpec* FindCs(const string &aSName);

	// Active CommandSpecs, ie those that have InputSources associated with them.
	// Only CommandSpecs and InputSources that are currently available at runtime
	// make it into this set.  
	typedef set<Controllable::CommandSpec*> CsSetType;
	CsSetType mSCsActive;
public:
	InputNexus() { mIIdCounter = 0; }
	virtual ~InputNexus();

	virtual unsigned char AddCommandSpec(Controllable::CommandSpec *aCs);
	// Find all the inputsources and put them in to the Is list.
	virtual void DiscoverInputSources() {}
	// Call Init just before the first call to ExecuteCommands.
	// (after FromNvt, DiscoverInputSources, or other setup methods)
	virtual void Init();

	virtual void ExecuteCommands(Controllable* aC);

	void AddInputSource(InputSource* aIn)
	{
		mLIsList.push_front(aIn);
	}

	virtual void ToOs(ostream &aos) const;
	virtual void FromIs(istream &aos);

	virtual void ToNvt(NVThing &aNvt) const;
	virtual void FromNvt(const NVThing &aNvt);
};


#endif
