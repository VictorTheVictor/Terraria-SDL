#ifndef NEAREST_NEIGHBOR_H_INCLUDED
#define NEAREST_NEIGHBOR_H_INCLUDED

#include <SDL.h>

#include "glob.h"

void drawTutorial(SDL_Surface *screen)
{
	stringColor(screen, 0, HEIGHT-10, "Press Mouse 1 to start game", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-20, "WASD to move", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-30, "Space to jump", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-40, "Shift to crouch/reduce speed", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-50, "CTRL to sprint", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-60, "1-9 to scroll through hotbar", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-70, "Mouse 1 to break blocks in game", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-80, "Mouse 2 to place blocks from inventory/hotbar", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-90, "-/= to zoom in and out", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-100, "Esc to pause", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-110, "F3 to cycle through the debug menu", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-120, "Press Esc to pause and unpause", 0xFFFFFFFF);
}

SDL_Surface *ScaleSurfaceNearest(SDL_Surface *src, int newW, int newH)
{
	if (!src || newW <= 0 || newH <= 0)
	{
		return NULL;
	}

	SDL_Surface *scaled = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		newW,
		newH,
		src->format->BitsPerPixel,
		src->format->Rmask,
		src->format->Gmask,
		src->format->Bmask,
		src->format->Amask
	);

	if (!scaled)
	{
		return NULL;
	}

	SDL_LockSurface(src);
	SDL_LockSurface(scaled);

	for (int y = 0; y < newH; y++)
	{
		int srcY = y * src->h / newH;

		for (int x = 0; x < newW; x++)
		{
			int srcX = x * src->w / newW;

			Uint8 *srcPixel = (Uint8 *)src->pixels + srcY * src->pitch + srcX * src->format->BytesPerPixel;
			Uint8 *dstPixel = (Uint8 *)scaled->pixels + y * scaled->pitch + x * scaled->format->BytesPerPixel;

			switch (src->format->BytesPerPixel)
			{
				case 1:
				{
					*dstPixel = *srcPixel;
					break;
				}

				case 2:
				{
					*(Uint16 *)dstPixel = *(Uint16 *)srcPixel;
					break;
				}

				case 3:
				{
					dstPixel[0] = srcPixel[0];
					dstPixel[1] = srcPixel[1];
					dstPixel[2] = srcPixel[2];
					break;
				}

				case 4:
				{
					*(Uint32 *)dstPixel = *(Uint32 *)srcPixel;
					break;
				}
			}
		}
	}

	SDL_UnlockSurface(scaled);
	SDL_UnlockSurface(src);

	return scaled;
}

#endif // NEAREST_NEIGHBOR_H_INCLUDED
