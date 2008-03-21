// Font.cpp by Luke Palmer
// For the Minesweeper Infinty project
// Implementation for class Font (Font.h)

#include "Font.h"
#include <cassert>
#include <iostream>

void renderFont(SDL_Surface* target,
		const string& ttfname, int ptsize,
		const string& text,
		int x, int y,
		Uint8 r, Uint8 g, Uint8 b)
{
	Font f(ttfname, ptsize);
	f.setColor(r,g,b);
	f.drawString(target, text, x, y);
}

Font::Font(const string& ttfname, int ptsize) 
{ 
	if (!TTF_WasInit()) {
		if (TTF_Init() == -1) {
			printf("Error initializing TTF library: %s\n", TTF_GetError());
		}
	}
	
	font_ = TTF_OpenFont(ttfname.c_str(), ptsize);
	color_.r = color_.g = color_.b = 255;
	
	if (font_ == NULL) {
		printf("Error opening %s: %s\n", ttfname.c_str(), TTF_GetError());
		exit(2);
	}
}

Font::~Font()
{
	TTF_CloseFont(font_);
}

int Font::getWidth(const string& text) const
{
	int width;
	TTF_SizeText(font_, text.c_str(), &width, NULL);
	return width;
}

int Font::getHeight(const string& text) const
{
	int height;
	TTF_SizeText(font_, text.c_str(), NULL, &height);
	return height;
}

int Font::getHeight() const
{
	// XXX guichan really should pass a string to get the height of
	// for now, we'll come up with a string that probably has
	// a large height:
	return getHeight("!Iljpq");
}

void Font::drawString(SDL_Surface* surf, const string& text, int x, int y)
{
	int width, height;
	TTF_SizeText(font_, text.c_str(), &width, &height);
	SDL_Rect target = { x, y, width, height };
	SDL_Surface* textSurface = TTF_RenderText_Blended(font_, text.c_str(), color_);
	SDL_BlitSurface(textSurface, NULL, surf, &target);
	SDL_FreeSurface(textSurface);

	SDL_UpdateRects(surf, 1, &target);
}

void Font::drawString(gcn::Graphics* graphics, const string& text, int x, int y)
{
	gcn::SDLGraphics* sdl = dynamic_cast<gcn::SDLGraphics*>(graphics);
	assert(sdl != 0);   // We only know how to handle SDL graphics

	int width, height;
	TTF_SizeText(font_, text.c_str(), &width, &height);
	SDL_Rect target = { x, y, width, height };
	SDL_Rect source = { 0, 0, width, height };
	SDL_Surface* textSurface = TTF_RenderText_Blended(font_, text.c_str(), color_);
	sdl->drawSDLSurface(textSurface, source, target);
	SDL_FreeSurface(textSurface);
}

void Font::setColor(Uint8 r, Uint8 g, Uint8 b) {
	color_.r = r;
	color_.g = g;
	color_.b = b;
}
