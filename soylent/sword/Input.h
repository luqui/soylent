#ifndef __INPUT_H__
#define __INPUT_H__

#include "manymouse.h"
#include <set>
#include <string>
#include <map>

class Input
{
public:
    Input() { }
    virtual ~Input() { }
    
    // do this every frame (for state checking, etc.)
    virtual void step () { }
    
    // visitor for all the kinds of events InputManager can send
    virtual void handle_SDL_Event(SDL_Event* e) { }
    virtual void handle_ManyMouseEvent(ManyMouseEvent* e) { }
private:
    // not safe to copy
    Input(const Input&);
    Input& operator = (const Input&);
};

class InputManager
{
public:
    InputManager() { }

    void add_input(Input* input) {
        inputs_.insert(input);
    }

    // note: does not delete pointer, just removes from set
    void remove_input(Input* input) {
        inputs_.erase(input);
    }

    void step() {
        step_sdl_events();
        step_manymouse_events();
    }

private:
    void step_sdl_events() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            for (inputs_t::iterator i = inputs_.begin(); i != inputs_.end(); ++i) {
                (*i)->handle_SDL_Event(&e);
            }
        }
    }

    void step_manymouse_events() {
        ManyMouseEvent e;
        while (ManyMouse_PollEvent(&e)) {
            for (inputs_t::iterator i = inputs_.begin(); i != inputs_.end(); ++i) {
                (*i)->handle_ManyMouseEvent(&e);
            }
        }
    }

    typedef std::set<Input*> inputs_t;
    inputs_t inputs_;
};


class KeyMapping
{
public:
    void bind(SDLKey key, std::string command) {
        keymap_[command] = key;
    }

    void unbind(std::string command) {
        keymap_.erase(command);
    }

    SDLKey key(std::string command) {
        return keymap_[command];  // returns 0 on failure, just what we want
    }
    
private:
    typedef std::map<std::string, SDLKey> keymap_t;
    keymap_t keymap_;
};

#endif
