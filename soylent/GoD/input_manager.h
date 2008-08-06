#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <iostream>

#include "SDL.h"

struct Input_Command
{ 
	Input_Command(SDL_Event *theEvents, int eventCount)
	{
		numEvents = eventCount;
		events = new SDL_Event[numEvents];
		for(int i = 0; i < numEvents; i++){
			events[i] = theEvents[i];
		}
	}

	~Input_Command()
	{
		delete[] events;
	}

	virtual void handle(SDL_Event *e) { return; }

	SDL_Event *events;
	int numEvents;

private:
    // no copying!
    Input_Command(const Input_Command&);
    Input_Command& operator= (const Input_Command&);
};

class Input_Manager
{
public:
	Input_Manager() 
	{
		keyDown_Bindings = new Input_Command *[SDLK_LAST];
		keyUp_Bindings = new Input_Command *[SDLK_LAST];
		for(int i = 0; i < SDLK_LAST; i++) {
			keyDown_Bindings[i] = NULL;
			keyUp_Bindings[i] = NULL;
		}
	}
	~Input_Manager() 
	{
        for (int i = 0; i < SDLK_LAST; i++) {
            delete keyDown_Bindings[i];
            delete keyUp_Bindings[i];
        }
		delete[] keyDown_Bindings;
		delete[] keyUp_Bindings;
	}

	void Register(Input_Command *command)
	{
		for(int i = 0; i < command->numEvents; i++) {
			switch(command->events[i].type)
			{
			case SDL_KEYDOWN:
                // maybe instead of unregistering the previous callback
                // we could assign it a new "Combination_Command" instance
                // which combines the effects of two commands.
                delete keyDown_Bindings[command->events[i].key.keysym.sym];
				keyDown_Bindings[command->events[i].key.keysym.sym] = command;
				break;
			case SDL_KEYUP:
                delete keyUp_Bindings[command->events[i].key.keysym.sym];
				keyUp_Bindings[command->events[i].key.keysym.sym] = command;
				break;
			default:
				std::cout<<"INPUT MANAGER REGISTRATION ERROR: Unhandled event type.\n";
				break;
			}
		}
	}

	int Manage()
	{
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			SDLKey key = e.key.keysym.sym;
			switch(e.type)
			{
			case SDL_KEYDOWN:
				if(key == SDLK_ESCAPE) return 0;
				else if(keyDown_Bindings[key] != NULL) keyDown_Bindings[key]->handle(&e);
				break;
			case SDL_KEYUP:
				if(keyUp_Bindings[key] != NULL) keyUp_Bindings[key]->handle(&e);
				break;
			default:
				break;
			}
		}
		return 1;
	}

private:

	Input_Command **keyDown_Bindings;
	Input_Command **keyUp_Bindings;

    // no copying!
    Input_Manager(const Input_Manager&);
    Input_Manager& operator= (const Input_Manager&);
};

#endif
