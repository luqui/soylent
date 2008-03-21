#ifndef FlockController_h
#define FlockController_h

#include "Controllable.h"
#include <time.h>
class RingMaster;


// Objects that distribute control to a set of controllables in various ways.

// FlockController can contain identical or different types of objects.
// It collects command specs from all objects, and passes commands to all 
// of its objects.
class FlockController : public Controllable
{
private:
	list<Controllable*> mLControllables;
public:
    virtual ~FlockController() { }
    
	void AddControllable(Controllable *aC);
	// Add the specs for commands that this object supports to a 
	// CommandSpecContainer.
    
	virtual void AddCommandSpecs(CommandSpecContainer *aCsc);

	virtual void ExecuteCommand (Command *aC);

	virtual void RemoveControllable(Controllable *aC);

};

// ControlSwitcher assumes identical or different objects, but only controls one at a time.
// It has a comand to change the currently 'active' object.  
class ControlSwitcher : public Controllable
{
private:
	vector<Controllable*> mVControllables;
	int mICurrent;
	clock_t mCtLast;
	clock_t mCtMinInterval;
	unsigned char mUcSwitch;
public:
	ControlSwitcher();
	void AddControllable(Controllable *aC);
	// Add the specs for commands that this object supports to a 
	// CommandSpecContainer.
	virtual void AddCommandSpecs(CommandSpecContainer *aCsc);

	virtual void ExecuteCommand (Command *aC);

	virtual void RemoveControllable(Controllable *aC);
};


// Multiplayer handles multiple objects.  Each object is presumed to belong to a player
// and a player prefix is assigned to each object's commands.
// Commands are routed to the objects depending on their prefix.
class MultiPlayer : public Controllable
{
public:
	MultiPlayer() {}
	void AddControllable(Controllable *aC, const char *aCPrefix);
	// Add the specs for commands that this object supports to a 
	// CommandSpecContainer.
	virtual void AddCommandSpecs(CommandSpecContainer *aCsc);

	virtual void ExecuteCommand (Command *aC);

	virtual void RemoveControllable(Controllable *aC);
private:
	vector<Controllable*> mVControllables;
	vector<string> mVPrefixes;

	class PrefixCsc : public CommandSpecContainer
	{
	public:
		PrefixCsc(CommandSpecContainer *aCsc, const char *aCPrefix);
        virtual ~PrefixCsc() { }
		virtual unsigned char AddCommandSpec(Controllable::CommandSpec *aCs);
		string mSPrefix;
	private:
		CommandSpecContainer *mCsc;
	};
};


#endif 
