#ifndef __UTILS_H__
#define __UTILS_H__

#include "config.h"
#include <string>
#include <stack>
#include <sstream>

SDL_Surface* cloneSurface(SDL_Surface* surface);
void addHighScore(int time, unsigned int seed, int width, int height, float density);
bool inRect(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, int xPix, int yPix);
void togglePaused();
void startGameClock();
void stopGameClock();

template <class t> std::string toString(t num) {
	std::stringstream ss;
	ss<<num;
	return ss.str();
}

class ScreenStack {
private:
	std::stack<SDL_Surface*> sStack;
public:
	~ScreenStack();
	void push(SDL_Surface* srcScreen);
	void pop(SDL_Surface* targetScreen);
	SDL_Surface* top() const;
};

#endif
