#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include "glob.h"
#include "texture.h"
#include "renderer.h"
#include "perlin_noise.h"
#include "nearest_neighbor.h"
#include "entity.h"

typedef enum BlockID
{
	AIR,
	DIRT,
	GRASS,
	STONE,
	BLOCK_ID_MAX
} BlockID;

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
	SDL_Surface* sprite;
	SDL_Surface* scaled_sprite;
	EntityControlSystem* entity_manager;
} Chunk;

Chunk* buildChunk(int x, int y)
{
	//printf ("X: %d, Y: %d\n", x, y);
	Chunk *chunk = malloc(sizeof(Chunk));
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
	chunk->sprite = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		256,
		256,
		32,
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		0xff000000,
		0x00ff0000,
		0x0000ff00,
		0x000000ff
#else
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
#endif
	);
	SDL_FillRect(
		chunk->sprite,
		NULL,
		SDL_MapRGBA(chunk->sprite->format, 0, 0, 0, 0)
	);
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			SDL_Rect dest_pos = { 16*i, 256-16*j-16, 0, 0 };
			if(chunk->block[i][j].id > 0)
				SDL_BlitSurface(blockImage[chunk->block[i][j].id], NULL, chunk->sprite, &dest_pos);
		}
	}
	//chunk->scaledSprite = ScaleSurfaceNearest(chunk->sprite, 16*renderer.standardBlockSize, renderer.standardBlockSize*16);
	//IMPLEMENT MIPMAPPING INSTEAD?
	/*char path[256];
	sprintf(path, "chunk_%d_%d.bmp", x, y);

	SDL_SaveBMP(chunk->sprite, path);*/
	return chunk;
}

Chunk* deleteChunk(Chunk* chunk)
{
	if (chunk->sprite) SDL_FreeSurface(chunk->sprite);
	return NULL;
}

Chunk *world[WORLD_SIZE][WORLD_SIZE];

void radialChunkPass(int player_x, int player_y)
{
	renderer.renderDistanceX = (int)ceilf((float)WIDTH/(16*renderer.standardBlockSize*2));
	renderer.renderDistanceY = (int)ceilf((float)HEIGHT/(16*renderer.standardBlockSize*2));
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
