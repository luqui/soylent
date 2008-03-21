//drawFunctions.h

#include "drawFunctions.h"
#include "gameBoard.h"
#include "Font.h"
#include "Clock.h"
#include "Utils.h"
#include <SDL_endian.h>	/* Used for the endian-dependent 24 bpp mode */
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <iomanip>
#include <fstream>
#include "SDL_rotozoom.h"

using namespace std;

// MapSurface: a reference-counting auto-destructing pointer to an SDL_Surface.
class MapSurface
{
  public:
    MapSurface() : image(0),refs(0) { }
    MapSurface(const string& file);
    MapSurface(const MapSurface& ref) : image(ref.image), refs(ref.refs) { ++*refs; }
    ~MapSurface() throw() { release(); }

    MapSurface& operator = (const MapSurface& ref);
    SDL_Surface* operator * () { if (!image) throw DrawError("Null surface used"); return image; }
    SDL_Surface* operator-> () { return **this; }

  private:
    void release();
  
    SDL_Surface* image;
    int* refs;
};

MapSurface::MapSurface(const string& file) : refs(new int(1))
{
	SDL_Surface* temp;
    temp = SDL_LoadBMP(file.c_str());
	image = zoomSurface(temp, scaleFactor, scaleFactor, smoothing);
    if (image == NULL) {
        throw DrawError("Couldn't load " + file + ": " + SDL_GetError());
    }
	SDL_FreeSurface(temp);
}

MapSurface& MapSurface::operator = (const MapSurface& ref)
{
    if (image == ref.image)
        return *this;
    release();
    image = ref.image;
    refs = ref.refs;
    ++*refs;
    return *this;
}

void MapSurface::release()
{
    if (refs && --*refs == 0) {
        delete refs;
        SDL_FreeSurface(image);
        image = 0;
    }
}

static map< string, MapSurface > bmpCache;




void showBMP(char *file, SDL_Surface *screen, int x, int y, bool update, bool cache)
{
    string sfile = file;
    MapSurface image;
    SDL_Rect dest;
    if (cache) {
        map< string, MapSurface >::iterator i = bmpCache.find(sfile);
        if (i != bmpCache.end()) {
            image = i->second;
        }
        else {
            pair< string, MapSurface > p(sfile, MapSurface(sfile));
            image = p.second;
            bmpCache.insert(p);
        }
    }
    else {
        image = MapSurface(sfile);
    }

    /* Blit onto the screen surface.
       The surfaces should not be locked at this point.
     */
    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(*image, NULL, screen, &dest);

    /* Update the changed portion of the screen */
    if(update)
		SDL_UpdateRects(screen, 1, &dest);
};

void drawBlock(SDL_Surface* targetScreen, int xSq, int ySq, int xPix, int yPix, bool update)
{
	if(win && minefield[xSq][ySq].lighted)
	{
		showBMP(ART_LIGHTSQ, targetScreen, xPix, yPix, update);
		return;
	}
		
	if(minefield[xSq][ySq].paused)
	{
		showBMP(ART_PAUSESQ, targetScreen, xPix, yPix, update);
		return;
	}

	if(minefield[xSq][ySq].covered)
	{
		if(minefield[xSq][ySq].flagged)
		{
			if(lose && !minefield[xSq][ySq].mined)
			{
				showBMP(ART_MISSFLAGSQ, targetScreen, xPix, yPix, update);
				return;
			}
			if(minefield[xSq][ySq].selected)
			{
				showBMP(ART_HIGHFLAGSQ, targetScreen, xPix, yPix, update);
				return;
			}
			showBMP(ART_FLAGSQ, targetScreen, xPix, yPix, update);
			return;
		}

		if(minefield[xSq][ySq].selected)
		{
			showBMP(ART_HIGHSQ, targetScreen, xPix, yPix, update);
			return;
		}
			
		if(lose && minefield[xSq][ySq].mined)
		{
			showBMP(ART_MINEDSQ, targetScreen, xPix, yPix, update);
			return;
		}

		showBMP(ART_COVEREDSQ, targetScreen, xPix, yPix, update);
		return;
	}
	
	if(lose)
	{
		if(minefield[xSq][ySq].mined)
		{
			showBMP(ART_THISMINEDSQ, targetScreen, xPix, yPix, update);
			return;
		}
	}

	
	switch(autocount ? minefield[xSq][ySq].displayNum : minefield[xSq][ySq].numMines)
	{
	case 0:
		showBMP(ART_EMPTYSQ, targetScreen, xPix, yPix, update);
		return;
	case 1:
		showBMP(ART_ONESQ, targetScreen, xPix, yPix, update);
		return;
	case 2:
		showBMP(ART_TWOSQ, targetScreen, xPix, yPix, update);
		return;
	case 3:
		showBMP(ART_THREESQ, targetScreen, xPix, yPix, update);
		return;
	case 4:
		showBMP(ART_FOURSQ, targetScreen, xPix, yPix, update);
		return;
	case 5:
		showBMP(ART_FIVESQ, targetScreen, xPix, yPix, update);
		return;
	case 6:
		showBMP(ART_SIXSQ, targetScreen, xPix, yPix, update);
		return;
	case 7:
		showBMP(ART_SEVENSQ, targetScreen, xPix, yPix, update);
		return;
	case 8:
		showBMP(ART_EIGHTSQ, targetScreen, xPix, yPix, update);
		return;
    default:
        showBMP(ART_NEGATIVESQ, targetScreen, xPix, yPix, update);
        return;
	}
};

void refreshGameBoard(SDL_Surface* targetScreen) {
	for(int i = 0; i < boardWidth; i++)
		for(int j = 0; j < boardHeight; j++)
			minefield[i][j].changed = true;
	drawGameBoard(targetScreen);
}

void drawGameBoard(SDL_Surface* targetScreen)
{	
	if(playing && !paused && !win && !lose)
		myclock.draw(targetScreen);

	if(oFlags != flags) drawMineCounter(targetScreen);

	/*if(win)
	{
		drawWinMessage();
	}*/
	
	if(!paused) {
		for (int xSq = 0; xSq < boardWidth; xSq++) {
			for (int ySq = 0; ySq < boardHeight; ySq++) {
				if (minefield[xSq][ySq].changed) {
					minefield[xSq][ySq].changed = false;
					drawBlock(targetScreen, xSq, ySq, sqToPix(xSq), sqToPix(ySq), false);
				}
			}
		}
		SDL_Flip(targetScreen);
	}
	else if(renderPaused) {
		SDL_Rect boardArea = {0, 0, boardWidth * squareLength, boardHeight * squareLength};
		SDL_FillRect(targetScreen, &boardArea, SDL_MapRGB(targetScreen->format, 0, 0, 0));
		SDL_Flip(targetScreen);
		
		Font pausedFont(ART_FONT, int(24*scaleFactor));  // XXX 24 (font point size)
		pausedFont.drawString(targetScreen, "paused", 
				  int(boardWidth * squareLength / 2 - pausedFont.getWidth("paused") / 2),
				  int(boardHeight * squareLength / 2 - tabHeight));
		renderPaused = false;
	}
};

void drawBoard(SDL_Surface* targetScreen)
{
	for(int x = 0; x < boardWidth; x++)
		for(int y = 0; y < boardHeight; y++)
			showBMP(ART_COVEREDSQ, screen, sqToPix(x), sqToPix(y), false);

	drawTab(targetScreen);
	myclock.redraw(targetScreen);
	drawMineCounter(targetScreen);

	SDL_Flip(targetScreen);
};

void drawTab(SDL_Surface* targetScreen)
{
	const int highlightWidth = 2 * scaleFactor;
	//showBMP(ART_BAR, targetScreen, 0, boardHeight * squareLength, false);//pretty blue bar at the bottom
	SDL_Rect* targetRect = new SDL_Rect;
	SDL_Rect* highlightRect = new SDL_Rect;
	highlightRect->x = clockWidth;
	highlightRect->y = boardHeight * squareLength;
	highlightRect->h = tabHeight;
	highlightRect->w = boardWidth * squareLength - clockWidth * 2;
	targetRect->x = highlightRect->x + highlightWidth;
	targetRect->y = highlightRect->y + highlightWidth;
	targetRect->h = highlightRect->h - 2 * highlightWidth;
	targetRect->w = highlightRect->w - 2 * highlightWidth;
	
	if (mouseX > highlightRect->x && 
		mouseX < highlightRect->x + highlightRect->w && 
		mouseY > highlightRect->y)
	{
		SDL_FillRect(targetScreen, highlightRect, SDL_MapRGB(targetScreen->format, 0, 255, 0));
		SDL_FillRect(targetScreen, targetRect, SDL_MapRGB(targetScreen->format, 31, 26, 85));
	}
	else {
		SDL_FillRect(targetScreen, highlightRect, SDL_MapRGB(targetScreen->format, 31, 26, 85));
	}
	
	for(int i = 28; i > 5; i--){
		Font optionsFont(ART_FONT, int(i*scaleFactor));  // XXX 24 (font point size)
		if(optionsFont.getWidth("options") <= targetRect->w && optionsFont.getHeight("options") <= targetRect->h) {
			optionsFont.drawString(targetScreen, "options", 
			                       int(boardWidth * squareLength / 2 - optionsFont.getWidth("options") / 2), 
			                       int(boardHeight * squareLength + tabHeight - optionsFont.getHeight("options")));
			break;
		}
	}
	

	delete highlightRect;
	delete targetRect;
}

void drawMineCounter(SDL_Surface* targetScreen)
{
	const int highlightWidth = 2 * scaleFactor;
	SDL_Rect* highlightRect = new SDL_Rect;
	SDL_Rect* blackRect = new SDL_Rect;
	highlightRect->x = 0;
	highlightRect->y = boardHeight * squareLength;
	highlightRect->h = tabHeight;
	highlightRect->w = clockWidth;
	SDL_FillRect(targetScreen, highlightRect, SDL_MapRGB(targetScreen->format, 0, 255, 0));
	blackRect->x = highlightRect->x + highlightWidth;
	blackRect->y = highlightRect->y + highlightWidth;
	blackRect->w = highlightRect->w - highlightWidth * 2;
	blackRect->h = highlightRect->h - highlightWidth * 2;
	if (mouseX > 0 && mouseX < clockWidth && mouseY > highlightRect->y) {
		SDL_FillRect(targetScreen, blackRect, SDL_MapRGB(targetScreen->format, 0, 0, 0));
	}
	else {
	showBMP(ART_CLOCK, targetScreen, 0, boardHeight * squareLength, false);
	}

	string num = toString(numMines - flags);
	Font mcFont(ART_FONT, int(24*scaleFactor));  // XXX 24 (font point size)
	int xstart = clockWidth - mcFont.getWidth(num) - int(5*scaleFactor);  // 5ish pixels gives the font some breathing room
	mcFont.drawString(targetScreen, num, xstart, int(boardHeight * squareLength + 3)); // XXX 3
	oFlags = flags;

	delete highlightRect;
	delete blackRect;
}

void drawWinMessage()
{}

void drawSearchingMessage(bool initialize) {
	static string text = "Searching";
	static Font searchingFont(ART_FONT, int(48*scaleFactor));
	static long dotTime = SDL_GetTicks();
	static int dotCounter = 0;
	static SDL_Surface* blank_board = 0;

	if(initialize) {
		dotTime = SDL_GetTicks();
		dotCounter = 0;
		if (blank_board) SDL_FreeSurface(blank_board);
		blank_board = cloneSurface(screen);
	}
	else if((SDL_GetTicks() - dotTime) > 1000) {
		switch(dotCounter) {
			case 0:
				text = "Searching";
				break;
			case 1:
				text = "Searching.";
				break;
			case 2:
				text = "Searching..";
				break;
			case 3:
				text = "Searching...";
				dotCounter = -1;
				break;
		}
		dotCounter++;
		SDL_BlitSurface(blank_board, NULL, screen, NULL);
		searchingFont.drawString(screen, text,
			boardWidth  * squareLength / 2 - searchingFont.getWidth(text) / 2,
			boardHeight * squareLength / 2 - searchingFont.getHeight(text) / 2);
		SDL_Flip(screen);
		dotTime = SDL_GetTicks();
	}
}
