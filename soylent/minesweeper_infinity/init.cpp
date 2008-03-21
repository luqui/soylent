//init.h

#include "init.h"
#include "GUI.h"
#include "Clock.h"
#include "Utils.h"
#include "Font.h"
#include "drawFunctions.h"
#include "gameBoard.h"
#include "Interpreter.h"
#include "SDL.h"
#include "SDL_gfxPrimitives.h"
#include "stdlib.h"
#include <cstdlib>
#ifndef NDEBUG
#include <iostream>
#endif

using namespace std;

void setup();

void initSDL()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);
	
	SDL_WM_SetCaption("Minesweeper Infinity",NULL);
	SDL_WM_SetIcon(SDL_LoadBMP(ART_WINDOW_ICON),NULL);
	screen = SDL_SetVideoMode(boardWidth * squareLength, boardHeight * squareLength + tabHeight, 24, SDL_HWSURFACE | SDL_RESIZABLE);
	if(screen == NULL)
	{
		fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
		exit(1);
	}
};

void initMinefield()
{
	int x, y;

	for(x = 0; x < boardWidth; x++)
		for(y = 0; y < boardHeight; y++)
		{
			minefield[x][y].flagged = false;
			minefield[x][y].mined = false;
			minefield[x][y].covered = true;
			minefield[x][y].numMines = 0;
			minefield[x][y].changed = false;
			minefield[x][y].selected = false;
			minefield[x][y].paused = false;
			minefield[x][y].lighted = false;
		};
	flags = 0;
        
        // This is a pretty dumb approximation to the bailout,
        // but it should do for now.
	bailout = (boardWidth * boardHeight * 10000) / (30);
};

void resetTestChanges(){
	int x, y;

	for(x = 0; x < boardWidth; x++)
		for(y = 0; y < boardHeight; y++)
		{
			minefield[x][y].flagged = false;
			//minefield[x][y].mined = false;
			minefield[x][y].covered = true;
			//minefield[x][y].numMines = 0;
			minefield[x][y].changed = false;
			minefield[x][y].selected = false;
			minefield[x][y].paused = false;
			minefield[x][y].lighted = false;
		};
	flags = 0;
        
    countMines();  // we remove flags, so we need to recompute displayNum
};

void setup()
{
	numMines = int(boardWidth * boardHeight * mineDensity);
	flags = 0;
	oFlags = 1;
	playing = false;
	bool keepTrying = true, resized = false;
	
	initSDL();
	myclock.reset();
	
	minefield = new block*[boardWidth];
	for(int i = 0; i < boardWidth; i++)
	{
		minefield[i] = new block[boardHeight];
	}

    screen = SDL_SetVideoMode(boardWidth * squareLength, boardHeight * squareLength + tabHeight, 24, SDL_HWSURFACE | SDL_RESIZABLE);
	drawBoard(screen);
	initMinefield();

	while(keepTrying)
	{
		selectedSquare(pixToSq(mouseX), pixToSq(mouseY), pixToSq(oldX), pixToSq(oldY));
		drawGameBoard(screen);
		drawTab(screen);//XXX this is a lazy way to make sure that the selected status of the tab is updated
		myclock.redraw(screen);
		drawMineCounter(screen);

		oldX = mouseX;
		oldY = mouseY;

		resized = getEvents();

		if(quit)
			exit(1);

		if(LMBFlag && inRange(pixToSq(mouseX), pixToSq(mouseY)))
		{
			keepTrying = false;
		}
		
		if(resized)
		{
			oFlags = 1;
			numMines = int(boardWidth * boardHeight * mineDensity);
			screen = SDL_SetVideoMode(boardWidth * squareLength, boardHeight * squareLength + tabHeight, 24, SDL_HWSURFACE | SDL_RESIZABLE);
			drawBoard(screen);
			resized = false;

			for(int i = 0; i < oldBoardWidth; i++)
			{
				delete[] minefield[i];
			}
			delete[] minefield;
			
			oldBoardWidth = boardWidth;
			oldBoardHeight = boardHeight;
      
			minefield = new block*[boardWidth];
			for(int i = 0; i < boardWidth; i++)
			{
				minefield[i] = new block[boardHeight];
			}

			initMinefield();
		}
	}
	
	oFlags = 1;//causes the mine count to be redrawn immediately rather than once the first flag is placed
#ifdef __WINDOWS__
	// Twiddling this flag keeps the screen from flashing after you click
	screen->flags &= ~SDL_RESIZABLE;
#else
	// But the flag twiddle does not have any effect on Linux, so
	// we have to resort to more drastic measures.
	screenStack.push(screen);
	screen = SDL_SetVideoMode(boardWidth * squareLength, boardHeight * squareLength + tabHeight, 
			24, SDL_HWSURFACE);
	screenStack.pop(screen);
	SDL_Flip(screen);
#endif
	
    int searched = 0;
	//mouseX and mouseY have already been established to be in range
    int startX = mouseX, startY = mouseY;

	drawSearchingMessage(true);//initialize it
	screenStack.push(screen);
	do{	
#ifndef NDEBUG
        std::cout << "\rBoards failed: " << searched++ << std::flush;
#endif
        initMinefield();
        seedMines(pixToSq(startX), pixToSq(startY));
        minefield[pixToSq(startX)][pixToSq(startY)].covered = false;
        minefield[pixToSq(startX)][pixToSq(startY)].changed = true;
        revealBlocks(pixToSq(startX), pixToSq(startY));
	}
	while(!testMinefield() && !quit);

#ifndef NDEBUG
    std::cout << "\rBoards failed: " << searched - 1 << "\n";
#endif
	screenStack.pop(screen);

	resetTestChanges();
	revealBlocks(pixToSq(startX), pixToSq(startY));

	//restore initial click state
	minefield[pixToSq(startX)][pixToSq(startY)].covered = false;
	minefield[pixToSq(startX)][pixToSq(startY)].changed = true;
	revealBlocks(pixToSq(startX), pixToSq(startY));

	playing = true;
	startGameClock();
};
