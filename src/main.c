#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>

#include "input_handling.h"
#include "chunk.h"
#include "scene_type.h"
#include "glob.h"

SDL_Surface *screen;

Uint32 mytimer(Uint32 ms, void *param) {
	SDL_Event ev;
	ev.type = SDL_USEREVENT;
	SDL_PushEvent(&ev);
	return ms;   /* wait this long till next call */
}

int main(int argc, char *argv[])
{
	srand (time(NULL));

	const float GRAVITY = 0.02;

	int debugMode = 1; //0 no debug, 1 variables, 2 screen position/rendering
	SceneType scene_type = TITLE_SCREEN; //0 title screen/controls, 1 game, 2 pause screen
	int mouseX = 0;
	int mouseY = 0;
	int worldTime = 10;
	bool canMoveUp, canMoveDown, canMoveLeft, canMoveRight;

	SDL_Event event;
	SDL_Surface *blockImageScaled[8];

	/* init SDL and open a window */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT);
	/* adding a timer for 20 ms; 1000 ms / 20 ms -> 50 fps */
	SDL_WM_SetCaption("Terraria", "Terraria");
	SDL_TimerID id = SDL_AddTimer(17, mytimer, NULL);

	loadTextures();

	//FILE *worldFile  = fopen("world.txt", "r+"); //TODO: SAVE FILE

    Entity *player = ECS_Spawn(spawnRegularEntity(33, 33, PLAYER));
    playerItemInit(player);
    ECS_Spawn(spawnRegularEntity(32, 32, SLIME));
    ECS_Spawn(spawnRegularEntity(31, 31, SLIME));

    printf("%d", ACTIVE_ECS.entity[0].entity_type);

	renderer.standardBlockSize = 16;

    radialChunkPass(CHUNK_POS(player->x), CHUNK_POS(player->y));

	bool game_loop = true;
	while (game_loop)
	{
		switch(scene_type)
		{
			//Title screen
			case TITLE_SCREEN:
				SDL_WaitEvent(&event);
				switch(event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						scene_type = GAME_SCREEN;
						break;
					case SDL_USEREVENT:
						drawTutorial(screen);
						break;
					case SDL_QUIT:
						game_loop = false;
						break;
				}
				SDL_Flip(screen);
				break;
			//Game
			case GAME_SCREEN:
				SDL_WaitEvent(&event);
				char bufferThing[32];
				switch (event.type)
				{
					/*case SDL_MOUSEBUTTONDOWN:
						if(!inventoryPress)
						{
							switch (event.button.button)
							{
								struct Blocks currently_selected_block = chunk
											[(int)((event.button.x - WIDTH/2)/(renderer.blockZoomFactor) + player->x + player->width/2) / 16]
											[(int)((HEIGHT/2 - event.button.y)/(renderer.blockZoomFactor) + player->y + player->height/2) / 16].block
											[(int)((event.button.x - WIDTH/2)/(renderer.blockZoomFactor) + player->x + player->width/2) % 16]
											[(int)((HEIGHT/2 - event.button.y)/(renderer.blockZoomFactor) + player->y + player->height/2) % 16];
								selBlockX = (int)((event.button.x - WIDTH/2)/(renderer.blockZoomFactor) + player->x + player->width/2) % 16;
								selBlockY = (int)((HEIGHT/2 - event.button.y)/(renderer.blockZoomFactor) + player->y + player->height/2) % 16;
								case SDL_BUTTON_LEFT:
									addItemToInventory(&player, currently_selected_block->id, 1);
									debugValue = currently_selected_block.id;
									break;
								case SDL_BUTTON_RIGHT:
									if(currently_selected_block.id == 0 &&
											player->inventory[player->hotbarSelect][0].count > 0)
									{
										 currently_selected_block.id = player->inventory[player->hotbarSelect][0].id;
										 player->inventory[player->hotbarSelect][0].count--;
										 if(player->inventory[player->hotbarSelect][0].count == 0) player->inventory[player->hotbarSelect][0].id = 0;
									}
									break;
							}
						}
						break;
					case SDL_MOUSEBUTTONUP:
						switch (event.button.button)
						{
							case SDL_BUTTON_LEFT:
								break;
							case SDL_BUTTON_RIGHT:
								break;
						}
						break;*/
					case SDL_MOUSEMOTION:
						mouseX = event.button.x;
						mouseY = HEIGHT-event.button.y;
						break;
					case SDL_KEYDOWN:
						processKeyRelease(event.key.keysym.sym);
						//sprintf(bufferThing, "Last key pressed: %d", event.key.keysym.sym);
						break;
					case SDL_KEYUP:
						processKeyPress(event.key.keysym.sym);
						break;
					case SDL_USEREVENT:
						//Physics

						//radialSwoop();
						//SET UP RECURSION
						//Y Movement
						//TODO: OOP like containerization
						//canMoveUp = true;
						/*for(int i = 0; i < player->height+1; i++)
						{
						{
							if(chunk[((int)(player->x + 0.1 + i*0.9) / 16)]
									[(int)((player->y + player->dy + player->height + 0.05) / 16)].
							   block[((int)(player->x + 0.1 + i*0.9) % 16)]
									[((int)(player->y + player->dy + player->height + 0.05) % 16)].id > 0)
								canMoveUp = false;
						}
						if(!canMoveUp)
						{
							if(player->dy > 0) player->dy = 0;
							player->y = round(player->y);
						}

						canMoveDown = true;
						for(int i = 0; i < player->height+1; i++)
						{
							if(chunk[((int)(player->x + 0.1 + i*0.9) / 16)]
									[(int)((player->y + player->dy - 0.05) / 16)].
							   block[((int)(player->x + 0.1 + i*0.9) % 16)]
									[((int)(player->y + player->dy - 0.05) % 16)].id > 0)
								canMoveDown = false;
						}
						if(!canMoveDown)
						{
							if(player->dy < -0.4) player->health += round(5*(player->dy));
							player->dy = 0;
							player->y = round(player->y);
							if(jumpPress == 1 && canMoveUp) player->dy = 0.5;
						}
						else
						{
							player->dy -= GRAVITY;
						}

						if(canMoveUp || canMoveDown) player->y += player->dy;

						//X Movement
						canMoveLeft = true;
						for(int i = 0; i < player->width+1; i++)
						{
							if(chunk[((int)(player->x - 0.05) / 16)]
									[(int)((player->y + i*0.9 + 0.1) / 16)].
							   block[((int)(player->x - 0.05) % 16)]
									[((int)(player->y + i*0.9 + 0.1) % 16)].id > 0 || player->x -0.05 < 0)
									canMoveLeft = false;
						}
						if(!canMoveLeft)
						{
							player->x = round(player->x);
						}

						canMoveRight = true;
						for(int i = 0; i < player->width+1; i++)
						{
							if(chunk[((int)(player->x + 0.05 + player->width) / 16)]
									[(int)((player->y + i*0.9 + 0.1) / 16)].
							   block[((int)(player->x + 0.05 + player->width) % 16)]
									[((int)(player->y + i*0.9 + 0.1) % 16)].id > 0 || player->x +0.05 > 256)
									canMoveRight = false;
						}
						if(!canMoveRight)
						{
							player->x = round(player->x);
						}

						if(canMoveLeft && rightPress == 0 && leftPress == 1) player->dx = -0.1;
						else if(canMoveRight && rightPress == 1 && leftPress == 0) player->dx = 0.1;
						else player->dx = 0;
						player->x += player->dx*player->dxMod;

						if(player->health <= 0)
						{
							player->health = 0;
							gameLoop = 0;
							break;
						}*/
						//RENDERING
						//Camera Zoom
						//renderer.standardBlockSize += keybind[ZOOM_IN];
						//renderer.standardBlockSize -= keybind[ZOOM_OUT];
						if(renderer.standardBlockSize > 100) renderer.standardBlockSize = 100;
						if(renderer.standardBlockSize < 1) renderer.standardBlockSize = 1;
						//if(keybind[RESET_CAMERA]) renderer.standardBlockSize = 25;

						//Draw Sky
						boxColor(screen, 0, 0, WIDTH, HEIGHT, SKY_COLOR);


						//for(int i = 0; i < 8; i++)
							//blockImageScaled[i] = ScaleSurfaceNearest(blockImage[i], renderer.standardBlockSize, renderer.standardBlockSize);

						for(int i = -renderer.renderDistanceX; i <= renderer.renderDistanceX; i++)
						{
							for(int j = -renderer.renderDistanceY; j <= renderer.renderDistanceY; j++)
							{
								//printf("i: %d, j: %d\n", i, j);
								SDL_Rect dest_pos = { i*256+renderer.renderDistanceX*256, HEIGHT-j*256-renderer.renderDistanceX*256};
								SDL_BlitSurface(world[WORLD_CENTER+i][WORLD_CENTER+j]->sprite, NULL, screen, &dest_pos);
							}
						}

						//Player
						for(int i = 0; i < ACTIVE_ECS.amount; i++)
						{
							int render_x = ACTIVE_ECS.entity[i].x*renderer.standardBlockSize;
							int render_y = ACTIVE_ECS.entity[i].y*renderer.standardBlockSize;
							boxColor(screen, render_x, HEIGHT-render_y, render_x+renderer.standardBlockSize, HEIGHT-render_y-renderer.standardBlockSize, entity_property[ACTIVE_ECS.entity[i].entity_type].color);
						}
						//hotbar
						/*boxColor(screen, (WIDTH-9*50)/2+player->hotbarSelect*50, HEIGHT,
										 (WIDTH-9*50)/2+(1+player->hotbarSelect)*50, HEIGHT - 50, 0x000000FF);*/
						for(int i = 0; i < 9; i++)
						{
							boxColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT, (WIDTH-9*50)/2+i*50 + 50, HEIGHT - 50, 0x88888888);
							if(player->inventory[i][0].id != 0 && player->inventory[i][0].id < 8)
							{
								SDL_Rect dest_pos = { (WIDTH-9*50)/2+i*50, HEIGHT - 50, 0, 0 };
								SDL_BlitSurface(blockImage[player->inventory[i][0].id], NULL, screen, &dest_pos);
								stringColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT - 10, intToString(player->inventory[i][0].amount), 0xFFFFFFFF);
							}
						}
						//inventory
						/*if(toggleState[INVENTORY])
						{
							for(int j = 0; j < 4; j++)
							{
								for(int i = 0; i < 9; i++)
								{
									boxColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT -j*50, (WIDTH-9*50)/2+i*50 + 50, HEIGHT -j*50 - 50, 0x88888888);
									if(player->inventory[i][j].id != 0 && player->inventory[i][j].id < 8)
									{
										SDL_Rect dest_pos = { (WIDTH-9*50)/2+i*50, HEIGHT -j*50 - 50, 0, 0 };
										SDL_BlitSurface(blockImage[player->inventory[i][j].id], NULL, screen, &dest_pos);
										stringColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT -j*50 - 10, intToString(player->inventory[i][j].count), 0xFFFFFFFF);
									}
								}
							}
						}*/

						//Health bar
						stringColor(screen, WIDTH-160, 0, "Health:", 0xFFFFFFFF);
						boxColor(screen, WIDTH-100, 0, WIDTH, 10, 0x333333FF);
						boxColor(screen, WIDTH-100, 0, WIDTH-5*(20-player->health), 10, 0xFF3333FF);
						for(int i = 0; i < 20; i++) rectangleColor(screen, WIDTH-5*(i), 0, WIDTH-5*(i), 10, 0x333333FF);

						//Screen debug info
						if(debugMode != 0) {
							//Screen position debugging
							if(debugMode == 2) {
								boxColor(screen, 0, 0, WIDTH/2, HEIGHT/2, 0xFF000088);
								boxColor(screen, WIDTH/2, HEIGHT/2, WIDTH, HEIGHT, 0x0000FF88);
							}
							//F3 menu
							char textBuffer[32];
							sprintf(textBuffer, "X: %f, Y: %f, Chunk X: %d, Chunk Y: %d", player->x, player->y, (int)((player->x) / 16), (int)((player->y) / 16));
							stringColor(screen, 0, 0, textBuffer, 0xFFFFFFFF);
							sprintf(textBuffer, "Zoom Level: %d", renderer.standardBlockSize);
							stringColor(screen, 0, 10, textBuffer, 0xFFFFFFFF);
							//stringColor(screen, 0, 20, bufferThing, 0xFFFFFFFF);
							sprintf(textBuffer, "Mouse X: %d, Mouse Y: %d", mouseX, mouseY);
							stringColor(screen, 0, 30, textBuffer, 0xFFFFFFFF);
							sprintf(textBuffer, "Speed X: %f, Speed Y: %f", player->dx, player->dy);
							stringColor(screen, 0, 40, textBuffer, 0xFFFFFFFF);
							sprintf(textBuffer, "Debug Value: %d", debugValue);
							stringColor(screen, 0, 50, textBuffer, 0xFFFFFFFF);
							sprintf(textBuffer, "Selected Block X: %d, Selected Block Y: %d", mouseX, mouseY);
							stringColor(screen, 0, 60, textBuffer, 0xFFFFFFFF);
						}

						break;
					case SDL_QUIT:
						game_loop = false;
						break;
				}
				SDL_Flip(screen);
				break;
			//Pause screen
			case PAUSE_SCREEN:
				SDL_WaitEvent(&event);
				switch(event.type)
				{
					case SDL_KEYDOWN:
						switch(event.key.keysym.sym)
						{
							case 27:
								scene_type = GAME_SCREEN;
								break;
						}
						break;
					case SDL_MOUSEBUTTONDOWN:
						break;
					case SDL_USEREVENT:
						drawTutorial(screen);
						break;
					case SDL_QUIT:
						game_loop = false;
						break;
				}
				SDL_Flip(screen);
				break;
		}
	}
	//fprintf(worldFile, "%f\n%f\n%f\n%f\n", player->x, player->y, player->dx, player->dy); //FILE SAVE

	SDL_RemoveTimer(id);
	SDL_Quit();
	return 0;
}
