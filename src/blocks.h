#ifndef BLOCKS_H
#define BLOCKS_H

typedef enum BlockID
{
	AIR,
	DIRT,
	GRASS,
	STONE,
	WATER,
	BLOCK_ID_MAX
} BlockID;

typedef struct
{
	float friction;
} BlockProperty;

static const BlockProperty blockProperty[BLOCK_ID_MAX] =
{
	[AIR] = {0.0f},
	[DIRT] = {1.0f},
	[GRASS] = {1.0f},
	[STONE] = {1.0f},
	[AIR] = {0.5f},
};

#endif // BLOCKS_H
