#ifndef __FONT_H__
#define __FONT_H__

// Font.h by Luke Palmer
// For the Minesweeper Infinity
// Implements a SDL bitmap font class

#include "config.h"
#include <string>
#include <guichan.hpp>
#include <guichan/sdl.hpp>

class Graphics;

using std::string;

void renderText(SDL_Surface* target, 
		        const string& ttfname, int ptsize, 
				const string& text,
				int x, int y, 
				Uint8 r = 255, Uint8 g = 255, Uint8 b = 255);

class Font : public gcn::Font
{
  public:
    Font(const string& ttfname, int ptsize);
    ~Font();

    int getWidth(const string& text) const;
    int getHeight() const;
	int getHeight(const string& text) const;

	void setColor(Uint8 r, Uint8 g, Uint8 b);

    void drawString(gcn::Graphics* graphics, const string& text, int x, int y);
	void drawString(SDL_Surface* surf, const string& text, int x, int y);

  private:
    TTF_Font* font_;
	SDL_Color color_;
};

#endif
