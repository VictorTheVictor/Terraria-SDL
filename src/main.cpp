#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_gfx/SDL3_gfxPrimitives.h>

#include "input_handling.h"
#include "chunk.h"
#include "scene_type.h"
#include "glob.h"

int main(int argc, char *argv[])
{
	srand (time(NULL));

	const float GRAVITY = 0.02;

	int debugMode = F3_MENU;
	SceneType scene_type = TITLE_SCREEN;
	int worldTime = 10;

	SDL_Event event;
	SDL_Surface *blockImageScaled[8];

	/* init SDL and open a window */
	SDL_Init(SDL_INIT_VIDEO );


	int displayCount = 0;
	SDL_DisplayID *displays = SDL_GetDisplays(&displayCount);
	if(!displayCount) return 1;

    for(int i = 0; i < displayCount; i++)
    {
        const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displays[i]);

        if(mode->w > maxWidth)
            maxWidth = mode->w;

        if(mode->h > maxHeight)
            maxHeight = mode->h;
    }

    SDL_free(displays);

	SDL_Window *window = SDL_CreateWindow("Pixelbuilder", 800, 600, SDL_WINDOW_RESIZABLE);

	screen = SDL_CreateRenderer(window, NULL);
	loadTextures();

	//FILE *worldFile  = fopen("world.txt", "r+"); //TODO: SAVE FILE

	Entity *player = ACTIVE_ECS.Spawn(Entity(PLAYER, Position(0, 0)));
	playerItemInit(player);

	Camera camera = Camera(player);

	ACTIVE_ECS.Spawn( Entity(SLIME, Position(34)) );
	ACTIVE_ECS.Spawn( Entity(SLIME, Position(32)) );

	renderer.standardBlockSize = 16;

	radialChunkPass(CHUNK_POS(player->position.x), CHUNK_POS(player->position.y));

	Uint32 lastFrameTime = SDL_GetTicks();

	float fps = 0.0f;
	float frameMs = 0.0f;

	float maxFps = 0.0f;

	int fps_index = 0;
	float avg_fps = 0.0f;
	float fps_array[256];

	bool game_loop = true;

	Uint32 lastTicks = SDL_GetTicks();

	while (game_loop)
	{
		Uint32 currentTicks = SDL_GetTicks();

		float deltaTick = 60 * (currentTicks - lastTicks) / 1000.0f;
		float deltaTime = (currentTicks - lastTicks);
		lastTicks = currentTicks;

		SDL_GetWindowSize(window, &WIDTH, &HEIGHT);

		switch(scene_type)
		{
			//Title screen
			case TITLE_SCREEN:
			{
				drawTutorial(screen);
				SDL_WaitEvent(&event);
				switch(event.type)
				{
					case SDL_EVENT_MOUSE_BUTTON_DOWN:
						scene_type = GAME_SCREEN;
						break;
					case SDL_EVENT_QUIT:
						game_loop = false;
						break;
				}
				SDL_RenderPresent(screen);
				break;
			}
			//Game
			case GAME_SCREEN:
			{
				SDL_PollEvent(&event);
				switch (event.type)
				{
					case SDL_EVENT_MOUSE_BUTTON_DOWN:
					{
						break;
					}
					case SDL_EVENT_MOUSE_MOTION:
					{
						mouse.Tick(scene_type, Point(event.button.x, HEIGHT-event.button.y));
						break;
					}

					case SDL_EVENT_KEY_DOWN:
					{
						keyboard.processKeyPress(event.key.scancode);
						break;
					}

					case SDL_EVENT_KEY_UP:
					{
						keyboard.processKeyRelease(event.key.scancode);
						break;
					}

					case SDL_EVENT_QUIT:
					{
						game_loop = false;
						break;
					}
				}

				keyboard.Tick();

				if (keyboard.key_properties[DEBUG].triggered) debugMode++;
				debugMode %= 3;

				if (keyboard.key_properties[CAMERA].triggered || camera.entity->active == INACTIVE)
					camera.entity = &ACTIVE_ECS.entity[ ACTIVE_ECS.Find_Next_Entity_Slot(ACTIVE, (int)(camera.entity-ACTIVE_ECS.entity)) ];

				player->internal.acceleration = POS_ZERO;

				if (keyboard.key_properties[MOVE_LEFT].pressed)
					player->internal.acceleration.x -= 0.5f;

				if (keyboard.key_properties[MOVE_RIGHT].pressed)
					player->internal.acceleration.x += 0.5f;

				if (keyboard.key_properties[JUMP].pressed)
					player->internal.acceleration.y += 0.5f;

				if (keyboard.key_properties[CROUCH].pressed)
					player->internal.acceleration.y -= 0.5f;

				if (camera.entity)
					camera.position = camera.entity->position;

				renderer.standardBlockSize += keyboard.key_properties[ZOOM_IN].triggered;
				renderer.standardBlockSize -= keyboard.key_properties[ZOOM_OUT].triggered;
				//if(renderer.standardBlockSize > 100) renderer.standardBlockSize = 100;
				//if(renderer.standardBlockSize < 1) renderer.standardBlockSize = 1;
				//if(keyboard.key_properties[RESET_CAMERA].triggered) renderer.standardBlockSize = 16;

				//Physics
				ACTIVE_ECS.Tick(deltaTick);
				//Y Movement
				//TODO: OOP like containerization
				//RENDERING
				//Camera Zoom

				//Draw Sky
				SDL_SetRenderTarget(screen, NULL);
				SDL_SetRenderDrawColor(screen, 00, 0x94, 255, 255);
				SDL_RenderClear(screen);


				//Draw terrain


				for(int i = -renderer.renderDistanceX; i <= renderer.renderDistanceX; i++)
				{
					for(int j = -renderer.renderDistanceY; j <= renderer.renderDistanceY; j++)
					{
						int x = (camera.position.x)*16;
						int y = (camera.position.y)*16;
						//printf("i: %d, j: %d\n", i, j);
						//NEW RECT. add the one after to that one and REUSE IT LATER
						SDL_FRect dest_pos =
						{
							(float)(i*256-camera.entity->position.x*16+WIDTH/2)-8,
							(float)(HEIGHT-256-j*256+camera.entity->position.y*16-HEIGHT/2+8),
							(float)(256),
							(float)(256)
						};
						SDL_RenderTexture(screen, world[WORLD_CENTER+i][WORLD_CENTER+j]->sprite, NULL, &dest_pos);
					}
				} //YET ANOTHER REFACTOR. LET'S GO //WAIT NO? WE'RE BACK

				//Draw entities
				for(int i = 0; i < ACTIVE_ECS.last_entity_index; i++)
				{
					int x = (ACTIVE_ECS.entity[i].position.x - camera.position.x)*16;
					int y = (ACTIVE_ECS.entity[i].position.y - camera.position.y)*16;
					boxColor(	screen,
								WIDTH/2+x-8,
								HEIGHT/2-y+8,
								WIDTH/2+x+8,
								HEIGHT/2-y-8,
								entity_property[ACTIVE_ECS.entity[i].entity_type].color);
				}
				//Draw hotbar
				for (int i = 0; i < 9; i++)
				{
					boxColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT, (WIDTH-9*50)/2+i*50 + 50, HEIGHT - 50, 0x88888888);
					if (player->inventory[i][0].id != 0 && player->inventory[i][0].id < 8)
					{
						SDL_FRect dest_pos = {(Sint16) ((WIDTH-9*50)/2+i*50), (Sint16)(HEIGHT) - 50, 0, 0 };
						SDL_RenderTexture(screen, blockImage[player->inventory[i][0].id], NULL, &dest_pos);
						stringColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT - 10, intToString(player->inventory[i][0].amount), 0xFFFFFFFF);
					}
				}
				//Draw inventory

				//Health bar
				stringColor(screen, WIDTH-160, 0, "Health:", 0xFFFFFFFF);
				boxColor(screen, WIDTH-100, 0, WIDTH, 10, 0x333333FF);
				boxColor(screen, WIDTH-100, 0, WIDTH-5*(20-player->health), 10, 0xFF3333FF);
				//Screen debug info
				if(debugMode != 0)
				{
					//Screen position debugging
					if(debugMode == 2)
					{
						boxColor(screen, 0, 0, WIDTH/2, HEIGHT/2, 0xFF000088);
						boxColor(screen, WIDTH/2, HEIGHT/2, WIDTH, HEIGHT, 0x0000FF88);
					}
					//F3 menu
					char textBuffer[256];
					sprintf(textBuffer, "X: %f, Y: %f, Chunk X: %d, Chunk Y: %d", player->position.x, player->position.y, CHUNK_POS(player->position.x), CHUNK_POS(player->position.y));
					stringColor(screen, 0, 0, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Zoom Level: %d", renderer.standardBlockSize);
					stringColor(screen, 0, 10, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Target Entity: %d", (int)(camera.entity - ACTIVE_ECS.entity));
					stringColor(screen, 0, 20, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Mouse X: %d, Mouse Y: %d", mouse.position.x, mouse.position.y);
					stringColor(screen, 0, 30, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Speed X: %f, Speed Y: %f", player->internal.speed.x, player->internal.speed.y);
					stringColor(screen, 0, 40, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Debug Value: %d", debugValue);
					stringColor(screen, 0, 50, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Selected Block X: %d, Selected Block Y: %d", mouse.position.x, mouse.position.y);
					stringColor(screen, 0, 60, textBuffer, 0xFFFFFFFF);

					fps = deltaTime > 0 ? 1000.0f / deltaTime : 0.0f;

					if (fps > maxFps) maxFps = fps;

					fps_array[fps_index] = fps;
					fps_index++;
					fps_index %=256;

					if(fps_index == 0)
					{
						avg_fps = 0;
						for(int i = 0; i < 256; i++)
							avg_fps += fps_array[i];
						avg_fps /= 256.0f;
					}

					sprintf(textBuffer, "FPS: %d", (int)fps);
					stringColor(screen, WIDTH-strlen(textBuffer) * 8 - 8, 15, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Avg. FPS: %.2f", avg_fps);
					stringColor(screen, WIDTH-strlen(textBuffer) * 8 - 8, 25, textBuffer, 0xFFFFFFFF);
					sprintf(textBuffer, "Frame Time: %d ms", (int)deltaTime);
					stringColor(screen, WIDTH-strlen(textBuffer) * 8 - 8, 35, textBuffer, 0xFFFFFFFF);

					for(int i = 0; i < 256; i++)
						lineRGBA(screen, WIDTH-256+i, 85, WIDTH-256+i, 85-40*fps_array[i]/maxFps, 255-255*fps_array[i]/maxFps, 255*fps_array[i]/maxFps, 0, 255);

				}
				SDL_RenderPresent(screen);
				break;
			}
			//Pause screen
			case PAUSE_SCREEN:
			{
				SDL_WaitEvent(&event);
				switch(event.type)
				{
					drawTutorial(screen);
					case SDL_EVENT_KEY_DOWN:
						switch(event.key.scancode)
						{
							case SDLK_ESCAPE:
								scene_type = GAME_SCREEN;
								break;
						}
						break;
					case SDL_EVENT_QUIT:
						game_loop = false;
						break;
				}
				SDL_RenderPresent(screen);
				break;
			}
		}
	}
	//fprintf(worldFile, "%f\n%f\n%f\n%f\n", player->position.x, player->position.y, player->dx, player->dy); //FILE SAVE

	SDL_Quit();
	return 0;
}
