#ifndef NEAREST_NEIGHBOR_H_INCLUDED
#define NEAREST_NEIGHBOR_H_INCLUDED

#include <SDL3/SDL.h>

#include "glob.h"

void drawTutorial(SDL_Renderer *screen)
{
	stringColor(screen, 0, HEIGHT-10, "Press Mouse 1 to start game", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-20, "WASD to move", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-30, "Space to jump", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-40, "Shift to crouch/fly down/reduce speed", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-50, "CTRL to sprint", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-60, "1-9 to scroll through hotbar", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-70, "Mouse 1 to break blocks in game", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-80, "Mouse 2 to place blocks from inventory/hotbar", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-90, "-/= to zoom in and out", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-100, "Esc to pause", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-110, "F3 to cycle through the debug menu", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-120, "Press Esc to pause and unpause", 0xFFFFFFFF);
	stringColor(screen, 0, HEIGHT-130, "F5 to switch spectating entity (player is default)", 0xFFFFFFFF);
}

#endif // NEAREST_NEIGHBOR_H_INCLUDED
