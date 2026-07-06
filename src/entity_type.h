#ifndef ENTITY_TYPE_H_INCLUDED
#define ENTITY_TYPE_H_INCLUDED

typedef enum EntityType
{
	PLAYER,
	SLIME,
	ARROW,
	ENTITY_TYPE_COUNT
} EntityType;

typedef struct EntityProperty
{
	float width, height;
	int defaultHealth;
	int color;
} EntityProperty;

static const EntityProperty entity_property[ENTITY_TYPE_COUNT] =
{
	[PLAYER] = {1.0f, 1.0f, 20, 0xFFFFFFFF},
	[SLIME] = {1.0f, 1.0f, 20, 0x00FFFFFF},
	[ARROW] = {1.0f, 1.0f, 20, 0x00FFFFFF},
};

#endif // ENTITY_TYPE_H_INCLUDED
