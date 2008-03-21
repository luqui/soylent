#ifndef Controllable_h
#define Controllable_h

#include "Port.h"

#include <string>
#include <list>
#include <vector>
#include "NVThing.h"
#include <time.h>

using namespace std;

// Controllable class.

// Characteristics of the Controllable class.
// - accepts command objects

class CommandSpecContainer;

class wxWindow;

// InputSources get input from things like keyboard keys, joystick axes, joystick buttons, etc.
// InputSources objects report the current state of these things with the 'IsActive' function.
class InputSource
{
public:
	// IsActive() Returns true if source is active, ie key is down or 
	// joystick axis is away from the dead zone, or whatever.
	// float does not need to be set to a value if no value is used for that cmd.
	// Valid float values are -1 to 1.
	virtual bool IsActive(float &aF) const = 0;
	virtual bool Init() = 0;

	// Relative or absolute motion?
	virtual bool IsRelative() const = 0;

	// KeyString must uniquely identify each InputSource.  Usually
	// "<type>-<spec>", like "Key-A" or "Joystick-axis-1"
	// This can also serve as a description of the InputSource.
	// Here 'Key' refers to a value in an index, not a keyboard key.
	virtual const string& KeyString() const = 0;

	// Used by wxWidgets specific descendants to create a control panel for the control.
	// For setting up dead zones, etc.  
	virtual wxWindow* CreateWxControlPanel() const { return 0; }

	// Some requirements for InputSource classes:
	// - must return input (of course)
	// - must be able to be created without the requisite hardware present, 
	// for configuration purposes. 
};

class Controllable
{
public:
	// Command message class.  controllables respond to these messages.
	class Command
	{
	public:
		unsigned char mUcId;
		float mFAmt;
		bool mBRelative;
	};
	// Class for specifying Commands that this object supports.
	class CommandSpec : public NVArchivable
	{
	public:
		string mSName;
		unsigned char mUcId;
		list<string> mLIsKeys;
		list<InputSource*> mLIsList;
		clock_t mCtMinTime;
		clock_t mCtLast;


		// Only specify the name.  ID is issued by the CommandSpecContainer.
		CommandSpec(const char *aCName, clock_t aCtMinTime = 0)
			:mSName(aCName), mUcId(0), mCtMinTime(aCtMinTime), mCtLast(0)
		{}

        virtual ~CommandSpec() { }

		void AddIs(InputSource* aIs) { mLIsList.push_front(aIs); }

		// This should pass commands to the Controllable
		// via Controllable::ExecuteCommand()
		virtual void ExecuteCommand(Controllable *aC);

		virtual void ToNvt(NVThing &aNvt) const;
		virtual void FromNvt(const NVThing &aNvt);
	};

    virtual ~Controllable() { }

	// Add the specs for commands that this object supports to a 
	// CommandSpecContainer.
	virtual void AddCommandSpecs(CommandSpecContainer *aCsc) = 0;

	virtual void ExecuteCommand (Command *aC) = 0;

	// If a controllable has been deleted, remove it if it is contained by this object.
	virtual void RemoveControllable(Controllable *aC) {}
};

// Prototype for objects that will hold commands that Controllables support.
// InputNexus is a CommandSpecContainer.
class CommandSpecContainer
{
public:
	virtual unsigned char AddCommandSpec(Controllable::CommandSpec *aCs) = 0;
};

#endif
