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
	char* name;
} BlockProperty;

static const BlockProperty blockProperty[BLOCK_ID_MAX] =
{
	[AIR] = 	{0.0f, "Air"},
	[DIRT] = 	{1.0f, "Dirt"},
	[GRASS] = 	{1.0f, "Grass block"},
	[STONE] = 	{1.0f, "Stone block"},
	[WATER] = 	{0.5f, "Water_tile"},
};

#endif // BLOCKS_H
