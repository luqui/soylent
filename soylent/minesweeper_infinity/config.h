#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <SDL.h>
#include <SDL/SDL_ttf.h>

#define MINES_SCORESDIR "scores"

#ifdef __WINDOWS__

#define ART_FLAGSQ         "art\\flagSq.bmp"
#define ART_MINEDSQ        "art\\minedSq.bmp"
#define ART_COVEREDSQ      "art\\coveredSq.bmp"
#define ART_THISMINEDSQ    "art\\thisMinedSq.bmp"
#define ART_MINEDSQ        "art\\minedSq.bmp"
#define ART_EMPTYSQ        "art\\emptySq.bmp"
#define ART_ONESQ          "art\\1Sq.bmp"
#define ART_TWOSQ          "art\\2Sq.bmp"
#define ART_THREESQ        "art\\3Sq.bmp"
#define ART_FOURSQ         "art\\4Sq.bmp"
#define ART_FIVESQ         "art\\5Sq.bmp"
#define ART_SIXSQ          "art\\6Sq.bmp"
#define ART_SEVENSQ        "art\\7Sq.bmp"
#define ART_EIGHTSQ        "art\\8Sq.bmp"
#define ART_HIGHSQ		   "art\\highSq.bmp"
#define ART_HIGHFLAGSQ	   "art\\highFlagSq.bmp"
#define ART_MISSFLAGSQ     "art\\missFlagSq.bmp"
#define ART_CLOCK		   "art\\clock.bmp"
#define ART_BAR			   "art\\bar.bmp"
#define ART_PAUSESQ		   "art\\pauseSq.bmp"
#define ART_LIGHTSQ		   "art\\brightSq.bmp"
#define ART_NEGATIVESQ     "art\\negativeSq.bmp"
#define ART_WINDOW_ICON	   "art\\MSI.bmp"
#define ART_FONT		   "art\\RATPOISO.TTF"

#else

#define ART_FLAGSQ         "art/flagSq.bmp"
#define ART_MINEDSQ        "art/minedSq.bmp"
#define ART_COVEREDSQ      "art/coveredSq.bmp"
#define ART_THISMINEDSQ    "art/thisMinedSq.bmp"
#define ART_MINEDSQ        "art/minedSq.bmp"
#define ART_EMPTYSQ        "art/emptySq.bmp"
#define ART_ONESQ          "art/1Sq.bmp"
#define ART_TWOSQ          "art/2Sq.bmp"
#define ART_THREESQ        "art/3Sq.bmp"
#define ART_FOURSQ         "art/4Sq.bmp"
#define ART_FIVESQ         "art/5Sq.bmp"
#define ART_SIXSQ          "art/6Sq.bmp"
#define ART_SEVENSQ        "art/7Sq.bmp"
#define ART_EIGHTSQ        "art/8Sq.bmp"
#define ART_HIGHSQ		   "art/highSq.bmp"
#define ART_HIGHFLAGSQ     "art/highFlagSq.bmp"
#define ART_MISSFLAGSQ     "art/missFlagSq.bmp"
#define ART_CLOCK		   "art/clock.bmp"
#define ART_BAR			   "art/bar.bmp"
#define ART_PAUSESQ		   "art/pauseSq.bmp"
#define ART_LIGHTSQ		   "art/brightSq.bmp"
#define ART_NEGATIVESQ     "art/negativeSq.bmp"
#define ART_WINDOW_ICON	   "art/MSI.bmp"
#define ART_FONT		   "art/RATPOISO.TTF"

#endif

#define HS_FILE			   "highscores.dat"

#define SMOOTHING_OFF	   0
#define SMOOTHING_ON	   1

struct block
{
	bool flagged, mined, covered, changed, selected, paused, lighted;
	int numMines, displayNum;
};

extern bool renderPaused;
extern int bailout;
extern int squareLength;
extern int tabHeight;
extern int clockWidth;
extern int mouseX;
extern int mouseY;
extern int LMBFlag;
extern int RMBFlag;
extern int oldX;
extern int oldY;
extern int boardWidth;
extern int boardHeight;
extern int oldBoardWidth;
extern int oldBoardHeight;
extern bool quit;
extern bool lose;
extern bool hasFocus;
extern bool win;
extern bool autocount;
extern bool flagClear;
extern bool rightClickInterface;
extern bool playing;
extern bool paused;
extern bool test;
extern int numMines;
extern int flags;
extern int oFlags;
extern double mineDensity;
extern double scaleFactor;
extern int smoothing;
extern struct SDL_Surface* screen;
extern block** minefield;
class Clock;
extern Clock myclock;
class ScreenStack;
extern ScreenStack screenStack;

#endif
