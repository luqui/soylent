//main.cpp

#include "config.h"
#include "Utils.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <exception>
#include "SDL.h"

#ifdef __WINDOWS__
#include <conio.h>
#endif

#include "init.h"
#include "gameBoard.h"
#include "drawFunctions.h"
#include "Font.h"
#include "GUI.h"
#include "Clock.h"

using namespace std;

int bailout = INT_MAX;

block** minefield;
SDL_Surface* screen;
Clock myclock;
ScreenStack screenStack;

bool renderPaused = false;
bool quit = false;
bool win = false;
bool lose = false;
bool autocount = true;
bool hasFocus = true;
bool playing = false;
bool paused = false;
bool playerAssist = false;
bool flagClear = true;
bool rightClickInterface = true;
int squareLength = int(18 * scaleFactor);  //pixels
int tabHeight = int(30 * scaleFactor);     //pixels
int clockWidth = int(60 * scaleFactor);    //pixels
int oldX;
int oldY;
int boardWidth = 30; //squares
int boardHeight = 16;//squares
int oldBoardWidth = boardWidth;
int oldBoardHeight = boardHeight;
int menuSizeX = 0;
int menuSizeY = boardHeight * squareLength;
int mouseX = squareLength * boardWidth / 2;
int mouseY = squareLength * boardHeight / 2;
int LMBFlag = 0;
int RMBFlag = 0;
double mineDensity = 99.1 / (30.0 * 16.0);
double scaleFactor = 1.25;
int smoothing = SMOOTHING_ON;
int numMines = int(boardWidth * boardHeight * mineDensity);
int flags;
int oFlags;
unsigned int seed = time(NULL);

void playGame();

int main(int argc, char *argv[])
{
	srand(seed);
    
#ifdef NDEBUG
    try {
#endif
	    setup();
	    playGame();
#ifdef NDEBUG
    }
    catch (const exception& x) {
        cerr << "Error: " << x.what() << endl;
    }
    catch (...) {
        cerr << "Unknown Error!  What the hell!?" << endl;
    }
#endif
	
	return 0;
}

void playGame()
{
	int oldLFlag, oldRFlag, xSq, ySq;

	if(scaleFactor == 1) smoothing = SMOOTHING_OFF;

	xSq = 0;
	ySq = 0;

	while(!quit)
	{
		oldLFlag = LMBFlag;
		oldRFlag = RMBFlag;
		oldX = xSq;
		oldY = ySq;
		
		getEvents();

		xSq = pixToSq(mouseX);
		ySq = pixToSq(mouseY);
		
		selectedSquare(xSq, ySq, oldX, oldY);

		if(inRange(xSq, ySq) && !paused)
		{
			if(rightClickInterface ? RMBFlag && !LMBFlag && !oldRFlag
								   :LMBFlag && !RMBFlag && !oldLFlag)
			{
				if(!minefield[xSq][ySq].covered)
					revealBlocks(xSq, ySq);
				if(minefield[xSq][ySq].numMines == 0 || (minefield[xSq][ySq].displayNum == 0 && flagClear))
					revealBlocks(xSq, ySq);

				if(!minefield[xSq][ySq].flagged && minefield[xSq][ySq].covered)
				{
					minefield[xSq][ySq].covered = false;
					minefield[xSq][ySq].changed = true;
				
					if(minefield[xSq][ySq].mined)
					{
						lose = true;
					}
				}
			}

            if(LMBFlag && RMBFlag) {
                if (!minefield[xSq][ySq].covered)
                    revealBlocks(xSq, ySq);
            }

			if(rightClickInterface ? LMBFlag && !RMBFlag && !oldLFlag
								  : RMBFlag && !LMBFlag && !oldRFlag)
			{
				if(minefield[xSq][ySq].covered)
				{
					minefield[xSq][ySq].flagged = !minefield[xSq][ySq].flagged;
					minefield[xSq][ySq].changed = true;

					if(minefield[xSq][ySq].flagged)
					{
						flags++;
						updateDisplayNum(xSq, ySq, true);
						//The is where the flagClear feature is implemented
						if(flagClear) {
							for(int x = -1; x < 2; ++x)
							{
								for(int y = -1; y < 2; ++y)
								{
									if(inRange(xSq + x, ySq + y))	
										if (!minefield[xSq + x][ySq +y].covered)
											revealBlocks(xSq + x, ySq +y);
								}
							}
						}//end of flagClear feature
					}
					
					else
					{
						flags--;
						updateDisplayNum(xSq, ySq, false);
					}
				}
			}
		}

		if(victory())
		{
			win = true;
			showFlags();
			flags = numMines;
			stopGameClock();
			addHighScore(myclock.clockNum(), seed, boardWidth, boardHeight, mineDensity);
		}

		if(lose) {
			stopGameClock();
			for(int x = 0; x < boardWidth; x++)
				for(int y = 0; y < boardHeight; y++)
					minefield[x][y].changed = true;
		}

		while(lose || win)
		{
			playing = false;
			if(quit)
				break;
			drawGameBoard(screen);
			oldLFlag = LMBFlag;

			getEvents();

			if(LMBFlag && LMBFlag != oldLFlag && inRange(pixToSq(mouseX), pixToSq(mouseY)))
			{
				lose = false;
				win = false;
				setup();	
			}
			SDL_Delay(10);//don't use all the processor
		}

		drawGameBoard(screen);
		drawTab(screen);//XXX this is a lazy way to make sure that the selected status of the tab is updated
		myclock.redraw(screen);
		drawMineCounter(screen);
		SDL_Delay(10);//don't hog the cpu you nasty minesweeper
	}
};
		
