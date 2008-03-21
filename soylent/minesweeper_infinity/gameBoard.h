#ifndef __GAMEBOARD_H__
#define __GAMEBOARD_H__

#include "config.h"

int sqToPix(int square);
int pixToSq(int pix);
bool inRange(int x, int y);
bool getEvents();
bool victory();
void clearArea(int x, int y);
bool checkNed(int x, int y, int nedX, int nedY);
void countMines();
void seedMines(int xSq, int ySq);
void revealBlocks(int x, int y);
void updateDisplayNum(int x, int y, bool flagged);
void showFlags();
void selectedSquare(int xSq, int ySq, int oldX, int oldY);

#endif
