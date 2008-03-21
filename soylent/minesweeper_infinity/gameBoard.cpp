//gameBoard.h

#include "Utils.h"
#include "gameBoard.h"
#include "drawFunctions.h"
#include "Interpreter.h"
#include "GUI.h"
#include "Clock.h"
#include <cstdlib>
#include <ctime>
#include "SDL.h"
#include <iostream>

using namespace std;

int sqToPix(int square)
{
	return square * squareLength;
}

int pixToSq(int pixel)
{
	return pixel / squareLength;
}

bool inRange(int x, int y)
{
	return 0 <= x && x < boardWidth  &&  0 <= y && y < boardHeight;
}

bool getEvents()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if(event.button.button == SDL_BUTTON_LEFT)
				LMBFlag = 1;
			if(event.button.button == SDL_BUTTON_RIGHT)
				RMBFlag = 1;
                        if(event.button.button == SDL_BUTTON_MIDDLE)
                                LMBFlag = RMBFlag = 1;

			mouseX = event.button.x;
			mouseY = event.button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			mouseX = event.button.x;
			mouseY = event.button.y;

			if(event.button.button == SDL_BUTTON_LEFT) {
				LMBFlag = 0;

				if(inTab(mouseX, mouseY));
				else if(myclock.inClock(mouseX, mouseY))
					togglePaused();
			}
			if(event.button.button == SDL_BUTTON_RIGHT)
				RMBFlag = 0;

            if(event.button.button == SDL_BUTTON_MIDDLE)
				LMBFlag = RMBFlag = 0;

			break;
		case SDL_MOUSEMOTION:
			mouseX = event.motion.x;
			mouseY = event.motion.y;
			break;
		case SDL_KEYUP:
			if(event.key.keysym.sym == SDLK_ESCAPE)
				quit = true;
			if(event.key.keysym.sym == SDLK_a)
			{
				autocount = !autocount;
				refreshGameBoard(screen);
			}
			if(event.key.keysym.sym == SDLK_f)
				flagClear = !flagClear;
			if(event.key.keysym.sym == SDLK_l)
				rightClickInterface = !rightClickInterface;
			if(event.key.keysym.sym == SDLK_SPACE) {
				loadGUIPage("Options");
			}
			if(event.key.keysym.sym == SDLK_s) {
				loadGUIPage("HighScores");
			}
			break;
		case SDL_VIDEORESIZE:
			if(event.resize.w < squareLength * 7)
				boardWidth = 7;
			else
				boardWidth = pixToSq(event.resize.w);//fitting to the base 18 grid
			if(event.resize.h < squareLength * 3 + tabHeight)
				boardHeight = 3;
			else
				boardHeight = pixToSq(event.resize.h - tabHeight);
                        if (boardWidth < 5) boardWidth = 5;
                        if (boardHeight < 5) boardHeight = 5;
                        if (boardWidth > 45) boardWidth = 45; // XXX magic numbers
                        if (boardHeight > 30) boardHeight = 30;
			return true;
		/*case SDL_ACTIVEEVENT:
			hasFocus = event.active.gain;
			break;*/
		case SDL_QUIT:
			quit = true;
			break;
		}
	}
        return false;
}

bool victory()
{
	for(int x = 0; x < boardWidth; x++)
		for(int y = 0; y < boardHeight; y++)
		{
			if(minefield[x][y].covered && !minefield[x][y].mined)
				return false;
		};
	return true;
}

void clearArea(int x, int y)
{
	if(x > 0)
	{
		if(minefield[x - 1][y].covered && !minefield[x - 1][y].flagged)
		{
			minefield[x - 1][y].covered = false;
			minefield[x - 1][y].changed = true;
			
			if(minefield[x - 1][y].mined)
				lose = true;

			if(minefield[x - 1][y].displayNum == 0)
				clearArea(x - 1, y);
		}

		if(y > 0)
		{
			if(minefield[x - 1][y - 1].covered && !minefield[x - 1][y - 1].flagged)
			{
				minefield[x - 1][y - 1].covered = false;
				minefield[x - 1][y - 1].changed = true;

				if(minefield[x - 1][y - 1].mined)
					lose = true;

				if(minefield[x - 1][y - 1].displayNum == 0)
					clearArea(x - 1, y - 1);
			}
		}

		if(y < boardHeight - 1)
		{
			if(minefield[x - 1][y + 1].covered && !minefield[x - 1][y + 1].flagged)
			{
				minefield[x - 1][y + 1].covered = false;
				minefield[x - 1][y + 1].changed = true;

				if(minefield[x - 1][y + 1].mined)
					lose = true;

				if(minefield[x - 1][y + 1].displayNum == 0)
					clearArea(x - 1, y + 1);
			}
		}
	}

	if(x < boardWidth - 1)
	{
		if(minefield[x + 1][y].covered && !minefield[x + 1][y].flagged)
		{
			minefield[x + 1][y].covered = false;
			minefield[x + 1][y].changed = true;

			if(minefield[x + 1][y].mined)
					lose = true;

			if(minefield[x + 1][y].displayNum == 0)
				clearArea(x + 1, y);
		}
	
		if(y > 0)
		{
			if(minefield[x + 1][y - 1].covered && !minefield[x + 1][y - 1].flagged)
			{
				minefield[x + 1][y - 1].covered = false;
				minefield[x + 1][y - 1].changed = true;

				if(minefield[x + 1][y - 1].mined)
					lose = true;

				if(minefield[x + 1][y - 1].displayNum == 0)
					clearArea(x + 1, y - 1);
			}
		}

		if(y < boardHeight - 1)
		{
			if(minefield[x + 1][y + 1].covered && !minefield[x + 1][y + 1].flagged)
			{
				minefield[x + 1][y + 1].covered = false;
				minefield[x + 1][y + 1].changed = true;

				if(minefield[x + 1][y + 1].mined)
					lose = true;

				if(minefield[x + 1][y + 1].displayNum == 0)
					clearArea(x + 1, y + 1);
			}
		}
	}

	if(y > 0)
	{
		if(minefield[x][y - 1].covered && !minefield[x][y - 1].flagged)
		{
			minefield[x][y - 1].covered = false;
			minefield[x][y - 1].changed = true;

			if(minefield[x][y - 1].mined)
					lose = true;

			if(minefield[x][y - 1].displayNum == 0)
				clearArea(x, y - 1);
		}
	}

	if(y < boardHeight - 1)
	{
		if(minefield[x][y + 1].covered && !minefield[x][y + 1].flagged)
		{
			minefield[x][y + 1].covered = false;
			minefield[x][y + 1].changed = true;

			if(minefield[x][y + 1].mined)
					lose = true;

			if(minefield[x][y + 1].displayNum == 0)
				clearArea(x, y + 1);
		}
	}
}

bool checkNed(int x, int y, int nedX, int nedY)
{
	if(x == nedX && y == nedY)
		return true;

	if(x > 0)
	{
		if(x - 1 == nedX && y == nedY)
			return true;

		if(y > 0)
			if(x - 1 == nedX && y - 1 == nedY)
				return true;
		
		if(y < boardHeight - 1)
		
			if(x - 1 == nedX && y + 1 == nedY)
				return true;
	}
	
	if(x < boardWidth - 1)
	{
		if(x + 1 == nedX && y == nedY)
			return true;
	
		if(y > 0)
			if(x + 1 == nedX && y - 1 == nedY)
				return true;

		if(y < boardHeight - 1)
			if(x + 1 == nedX && y + 1 == nedY)
				return true;

	}

	if(y > 0)
		if(x == nedX && y - 1 == nedY)
			return true;

	if(y < boardHeight - 1)
		if(x == nedX && y + 1 == nedY)
			return true;

	return false;
}

void countMines()
{
	for(int x = 0; x < boardWidth; x++)
		for(int y = 0; y < boardHeight; y++)
		{
			minefield[x][y].numMines = 0;
			if(x != 0)
			{
				if(minefield[x - 1][y].mined)
					minefield[x][y].numMines++;
				if(y != 0)
					if(minefield[x - 1][y - 1].mined)
						minefield[x][y].numMines++;
				if(y != boardHeight - 1)
					if(minefield[x - 1][y + 1].mined)
						minefield[x][y].numMines++;
			}

			if(x != boardWidth - 1)
			{
				if(minefield[x + 1][y].mined)
					minefield[x][y].numMines++;
				if(y != 0)
					if(minefield[x + 1][y - 1].mined)
						minefield[x][y].numMines++;
				if(y != boardHeight - 1)
					if(minefield[x + 1][y + 1].mined)
						minefield[x][y].numMines++;
			}

			if(y != 0)
				if(minefield[x][y - 1].mined)
					minefield[x][y].numMines++;
			if(y != boardHeight - 1)
				if(minefield[x][y + 1].mined)
					minefield[x][y].numMines++;
			if(minefield[x][y].mined)
				minefield[x][y].numMines++;

			minefield[x][y].displayNum = minefield[x][y].numMines;
		}	
}

void seedMines(int xSq, int ySq)
{
	numMines = int(boardWidth * boardHeight * mineDensity);
	if(numMines > 999)
		numMines = 999;
	int c = 0;
	int x = 0;
	int y = 0;
	while(c < numMines)
	{
		x = rand() % boardWidth;
		y = rand() % boardHeight;
		if(!minefield[x][y].mined)
		{
			//check to see that block (x,y) is not the square clicked
                        //(just prunes the number of boards that must be searched)
			if(!(xSq == x && ySq == y))
			{
				minefield[x][y].mined = true;
				c++;
			}
		}
	}
	countMines();
}

void revealBlocks(int x, int y)
{
	if(minefield[x][y].displayNum == 0)//if all mines surrounding the number are flagged
            clearArea(x, y);
}

void updateDisplayNum(int x, int y, bool flagged)
{
	if(x != 0)
	{
		if(flagged)
			minefield[x - 1][y].displayNum--;
		else
			minefield[x - 1][y].displayNum++;

		minefield[x - 1][y].changed = true;

		if(y != 0)
		{
			if(flagged)
				minefield[x - 1][y - 1].displayNum--;
			else
				minefield[x - 1][y - 1].displayNum++;

			minefield[x - 1][y - 1].changed = true;
		}

		if(y != boardHeight - 1)
		{
			if(flagged)
				minefield[x - 1][y + 1].displayNum--;
			else
				minefield[x - 1][y + 1].displayNum++;

			minefield[x - 1][y + 1].changed = true;
		}
	}

	if(x != boardWidth - 1)
	{
		if(flagged)
			minefield[x + 1][y].displayNum--;
		else
			minefield[x + 1][y].displayNum++;

		minefield[x + 1][y].changed = true;

		if(y != 0)
		{
			if(flagged)
				minefield[x + 1][y - 1].displayNum--;
			else
				minefield[x + 1][y - 1].displayNum++;

			minefield[x + 1][y - 1].changed = true;
		}

		if(y != boardHeight - 1)
		{
			if(flagged)
				minefield[x + 1][y + 1].displayNum--;
			else
				minefield[x + 1][y + 1].displayNum++;

			minefield[x + 1][y + 1].changed = true;
		}
	}

	if(y != 0)
	{
		if(flagged)
			minefield[x][y - 1].displayNum--;
		else
			minefield[x][y - 1].displayNum++;

		minefield[x][y - 1].changed = true;
	}

	if(y != boardHeight - 1)
	{
		if(flagged)
			minefield[x][y + 1].displayNum--;
		else
			minefield[x][y + 1].displayNum++;

		minefield[x][y + 1].changed = true;
	}
}

void showFlags()
{
	for(int x = 0; x < boardWidth; x++)
	{
		for(int y = 0; y < boardHeight; y++)
		{
			if(minefield[x][y].mined)
			{
				if(!minefield[x][y].flagged)
				{
					minefield[x][y].flagged = true;
					minefield[x][y].changed = true;
					updateDisplayNum(x, y, true);
				}
			}
		}
	}
}

void selectedSquare(int xSq, int ySq, int oldX, int oldY)
{
	if(inRange(oldX, oldY))
	{
		minefield[oldX][oldY].selected = false;
		minefield[oldX][oldY].changed = true;
	}
	if(inRange(xSq, ySq) && hasFocus)
	{
		minefield[xSq][ySq].selected = true;
		minefield[xSq][ySq].changed = true;
	}
}
