#include "Utils.h"
#include "Clock.h"
#include "GUI.h"
#include "HighScoreSheet.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

SDL_Surface* cloneSurface(SDL_Surface* surface)
{
	SDL_Surface* returnSurface = SDL_ConvertSurface(surface, surface->format, surface->flags);
	return returnSurface;
}

void addHighScore(int score, unsigned int seed, int width, int height, float density)
{
	HighScoreSheet* scores_ = new LocalHighScoreSheet;
#ifdef WIN32
	HighScoreEntry entry(score, getenv("USERNAME"), time(NULL), seed, width, height, density);
#else
	HighScoreEntry entry(score, getenv("USER"), time(NULL), seed, width, height, density);
#endif
	if(entry < scores_->min_entry()) {
		scores_->insert(entry);
		loadGUIPage("HighScores");
	}
}

bool inRect(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, int x, int y) {
	return topLeftX < x && bottomRightX > x && topLeftY < y && bottomRightY > y;
}

void togglePaused() {
	if(playing){
		if(paused) {
			startGameClock();
			paused = false;
		}
		else {
			stopGameClock();
			paused = true;
			renderPaused = true;
		}
		for(int i = 0; i < boardWidth; i++)
			for(int j = 0; j < boardHeight; j++)
			{
				minefield[i][j].paused = !minefield[i][j].paused;
				minefield[i][j].changed = true;
			}
	}
}

void startGameClock() {
	myclock.start();
}

void stopGameClock() {
	myclock.stop();
}

void ScreenStack::push(SDL_Surface* srcScreen) {
	sStack.push(cloneSurface(srcScreen));
}
void ScreenStack::pop(SDL_Surface* targetScreen) {
	SDL_Surface* tempSurface = sStack.top();
	SDL_BlitSurface(tempSurface, NULL, targetScreen, NULL);
	SDL_FreeSurface(tempSurface);
	sStack.pop();
}
ScreenStack::~ScreenStack() {
	while(sStack.size()) {
		SDL_FreeSurface(sStack.top());
		sStack.pop();
	}
}

SDL_Surface* ScreenStack::top() const {
	return sStack.top();
}
