#ifndef __DRAWFUNCTIONS_H__
#define __DRAWFUNCTIONS_H__

#include "config.h"
#include "SDL.h"
#include <exception>
#include <string>

void showBMP(char* file, SDL_Surface* screen, int x, int y, 
             bool update = true, bool cache = true);
void drawBlock(SDL_Surface* targetScreen, int xSq, int ySq, int xPix, int yPix, bool update = true);
void refreshGameBoard(SDL_Surface* targetScreen);
void drawGameBoard(SDL_Surface* targetScreen);
void drawBoard(SDL_Surface* targetScreen);
void drawTab(SDL_Surface* targetScreen);
void drawMineCounter(SDL_Surface* targetScreen);
void drawHighScores();
void drawWinMessage();
void drawSearchingMessage(bool initialize = false);

class DrawError : public std::exception
{
  public:
    DrawError(const std::string& wh) : description(wh) { }
    const char* what() const throw() { return description.c_str(); }
    ~DrawError() throw() { }
  private:
    std::string description;
};

#endif
