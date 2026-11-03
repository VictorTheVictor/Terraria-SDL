#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_gfx/SDL3_gfxPrimitives.h>

#include "glob.h"
#include "texture.h"
#include "renderer.h"
#include "perlin_noise.h"
#include "nearest_neighbor.h"
#include "entity.h"
#include "blocks.h"

#define CHUNK_DEBUG 0

typedef struct
{
	BlockID id;
	char lightLevel;
	char damageLevel;
} Block;
//NEED seed system
typedef struct
{
	Block block[16][16];
	int x, y;
	Entity *entity;
	SDL_Texture* sprite;
	SDL_Texture* scaled_sprite;
	EntityControlSystem* entity_manager;
} Chunk;

Chunk* buildChunk(int x, int y)
{
	//printf ("X: %d, Y: %d\n", x, y);
	Chunk *chunk = new Chunk;
	for(int i = 0; i < 16; i++)
	{
		float noise = FractalPerlin1D((float)(REAL_X) * 0.025f, 5, 0.5f);
		int baseHeight = 64; //sea level?
		int hillHeight = 96;

		int groundY = baseHeight + (int)(noise * hillHeight);

		for(int j = 0; j < 16; j++)
		{
			if( REAL_Y > groundY) // TARGET BLOCK ABOVE GROUND LEVEL
			{
				chunk->block[i][j].id = AIR;
			}
			else if(REAL_Y == groundY) //TARGET BLOCK AT GROUND LEVEL
			{
				chunk->block[i][j].id = GRASS;
			}
			else if (REAL_Y < groundY && REAL_Y >= groundY-15) //TARGET BLOCK BELOW GROUND LEVEL AND ABOVE 15
			{
				chunk->block[i][j].id = DIRT;
			}
			else if (REAL_Y < groundY-15) //TARGET BLOCK BELOW GROUND LEVEL AND BELOW 15
			{
				chunk->block[i][j].id = STONE;
			}
			/*chunk->block[i][j].lightLevel = 16 - (groundY - REAL_Y);
			if (chunk->block[i][j].lightLevel > 16)
				chunk->block[i][j].lightLevel = 16;
			if (chunk->block[i][j].lightLevel < 0)
				chunk->block[i][j].lightLevel = 0;*/
			chunk->block[i][j].lightLevel = 16;

			if(x == 0) {chunk->block[i][j].id =  GRASS; }
		}
	}
	chunk->sprite = SDL_CreateTexture(
		screen,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		256,
		256
	);

	SDL_SetRenderTarget(screen, chunk->sprite);

	SDL_SetRenderDrawColor(screen, 0, 0, 0, 0);
	SDL_RenderClear(screen);


	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			SDL_FRect dest_pos = { (float)(16*i), (float)(256-16*j-16), (float)16, (float)16 };
			if(chunk->block[i][j].id > 0)
				SDL_RenderTexture(screen, blockImage[chunk->block[i][j].id], NULL, &dest_pos);
		}
	}

	if (CHUNK_DEBUG == 1)
	{
		for(int i = 0; i < 16; i++)
			for(int j = 0; j < 16; j++)
				rectangleColor(screen, i*16, j*16, i*16+16, j*16+16, 0x000000FF);
		rectangleColor(screen, 0, 0, 256, 256, 0x000000FF);
		rectangleColor(screen, 1, 1, 255, 255, 0x000000FF);
		char textBuffer[256];
		sprintf(textBuffer, "X: %d, Y: %d", x, y);
		stringColor(screen, 10, 10, textBuffer, 0xFFFFFFFF);
	}

	//chunk->scaledSprite = ScaleSurfaceNearest(chunk->sprite, 16*renderer.standardBlockSize, renderer.standardBlockSize*16);
	//IMPLEMENT MIPMAPPING INSTEAD?
	/*char path[256];
	sprintf(path, "chunk_%d_%d.bmp", x, y);

	SDL_SaveBMP(chunk->sprite, path);*/

	SDL_SetRenderTarget(screen, NULL);

	return chunk;
}

Chunk* deleteChunk(Chunk* chunk)
{
	if (chunk->sprite) SDL_DestroyTexture(chunk->sprite);
	return NULL;
}

Chunk *world[WORLD_SIZE][WORLD_SIZE];

void radialChunkPass(float player_x, float player_y)
{
	renderer.renderDistanceX = (int)ceilf(WIDTH/(16*renderer.standardBlockSize*2.0f));
	renderer.renderDistanceY = (int)ceilf(HEIGHT/(16*renderer.standardBlockSize*2.0f));
	//printf ("Render X: %d, Render Y: %d\n", renderer.renderDistanceX, renderer.renderDistanceY);
	int x = 0;
	int y = 0;

	if (!world[WORLD_CENTER+x][WORLD_CENTER+y])
		world[WORLD_CENTER+x][WORLD_CENTER+y] = buildChunk(player_x+x, player_y+y);

	int stepLength = 1;

	while (abs(x) <= renderer.renderDistanceX || abs(y) <= renderer.renderDistanceY)
	{
		for (int i = 0; i < stepLength; i++)
		{
			y--;
			if (!world[WORLD_CENTER+x][WORLD_CENTER+y] && abs(x) <= renderer.renderDistanceX && abs(y) <= renderer.renderDistanceY)
				world[WORLD_CENTER+x][WORLD_CENTER+y] = buildChunk(player_x+x, player_y+y);
		}

		for (int i = 0; i < stepLength; i++)
		{
			x++;
			if (!world[WORLD_CENTER+x][WORLD_CENTER+y] && abs(x) <= renderer.renderDistanceX && abs(y) <= renderer.renderDistanceY)
				world[WORLD_CENTER+x][WORLD_CENTER+y] = buildChunk(player_x+x, player_y+y);
		}
		stepLength++;

		for (int i = 0; i < stepLength; i++)
		{
			y++;
			if (!world[WORLD_CENTER+x][WORLD_CENTER+y] && abs(x) <= renderer.renderDistanceX && abs(y) <= renderer.renderDistanceY)
				world[WORLD_CENTER+x][WORLD_CENTER+y] = buildChunk(player_x+x, player_y+y);
		}

		for (int i = 0; i < stepLength; i++)
		{
			x--;
			if (!world[WORLD_CENTER+x][WORLD_CENTER+y] && abs(x) <= renderer.renderDistanceX && abs(y) <= renderer.renderDistanceY)
				world[WORLD_CENTER+x][WORLD_CENTER+y] = buildChunk(player_x+x, player_y+y);
		}
		stepLength++;
	}
}

#endif // CHUNK_H_INCLUDED
