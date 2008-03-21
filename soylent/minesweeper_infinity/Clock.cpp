#include "Clock.h"
#include "drawFunctions.h"
#include "Font.h"
#include "Utils.h"
#include <cassert>
#include <sstream>
#include <iomanip>

Clock::Clock() {
	reset();
}

int Clock::clockNum() {
	if(stopCounter == 0) return (SDL_GetTicks() - startTicks) / 1000;
	else                 return (stopTicks      - startTicks) / 1000;
}

void Clock::start() {
	assert(stopCounter > 0);

	if(stopCounter == 1) {
		startTicks += SDL_GetTicks() - stopTicks;
	}
	stopCounter--;
}
void Clock::stop() {
	if(stopCounter == 0) {
		stopTicks = SDL_GetTicks();
	}
	stopCounter++;
}
void Clock::reset() {
	startTicks = 0;
	stopTicks = 0;
	stopCounter = 1;
	lastClock = -1;
}
void Clock::draw(SDL_Surface* drawSurface) {
	if (clockNum() > lastClock) redraw(drawSurface);
}
void Clock::redraw(SDL_Surface* drawSurface) {
	const int highlightWidth = 2 * scaleFactor;
	SDL_Rect* highlightRect = new SDL_Rect;
	SDL_Rect* blackRect = new SDL_Rect;
	highlightRect->x = boardWidth * squareLength - clockWidth;
	highlightRect->y = boardHeight * squareLength;
	highlightRect->h = tabHeight;
	highlightRect->w = clockWidth;
	SDL_FillRect(drawSurface, highlightRect, SDL_MapRGB(drawSurface->format, 0, 255, 0));

	int drawNum = clockNum();
	if(drawNum > 999) {
		startTicks = SDL_GetTicks() - 999 * 1000;
		drawNum = 999;
	}
	
	blackRect->x = highlightRect->x + highlightWidth;
	blackRect->y = highlightRect->y + highlightWidth;
	blackRect->w = highlightRect->w - highlightWidth * 2;
	blackRect->h = highlightRect->h - highlightWidth * 2;
	if (mouseX > highlightRect->x && mouseX < highlightRect->x + highlightRect->w && mouseY > highlightRect->y) {
		SDL_FillRect(drawSurface, blackRect, SDL_MapRGB(drawSurface->format, 0, 0, 0));
	}
	else {
		showBMP(ART_CLOCK, drawSurface, boardWidth * squareLength - clockWidth, boardHeight * squareLength, false);
	}
	
	string num = toString(drawNum);
	lastClock = drawNum;

	Font clockFont(ART_FONT, 24*scaleFactor);
	int xstart = int(boardWidth * squareLength) - clockFont.getWidth(num) - int(5*scaleFactor);
	clockFont.drawString(drawSurface, num, xstart, 
				   int(boardHeight * squareLength + 2.5 * scaleFactor)); // XXX 2.5 means what?

	delete blackRect;
	delete highlightRect;
}

bool Clock::inClock(int x, int y) {
	return inRect(boardWidth * squareLength - clockWidth, boardHeight * squareLength,
				  boardWidth * squareLength, boardHeight * squareLength + tabHeight, 
				  x, y);
}
