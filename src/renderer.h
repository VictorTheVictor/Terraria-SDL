#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL.h>

struct
{
	int renderDistanceX, renderDistanceY;
	unsigned int standardBlockSize;
} renderer;

SDL_Rect gameSpaceToScreenSpace (int x, int y)
{
	SDL_Rect result;
	return result;
}

void drawBox(SDL_Surface *screen, int x, int y, int boxSize, int color)
{
	boxColor(screen, x, HEIGHT-y, x+boxSize, HEIGHT-y-boxSize, color);
}

#endif // RENDERER_H_INCLUDED
