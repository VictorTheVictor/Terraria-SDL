#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#define MAX_ENTITY_POOL 256

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "item.h"
#include "entity_type.h"

#define POS_ZERO ((Position){0.0f,0.0f})
#define POS(x) ((Position){(float)(x),(float)(x)})
#define POS_SET(x,y) ((Position){(float)(x),(float)(y)})

struct Position
{
        float x, y;
        Position(float _x, float _y) : x(_x), y(_y) {}
        Position(int _x, int _y) : x((int)_x), y((int)_y) {}
        Position(float _a) : x(_a), y(_a) {}
        Position(int _a) : x((int)_a), y((int)_a) {}
        Position() : x(0), y(0) {}
};

#define FLOAT_CLAMP(x,max_x) (x)=fmaxf(-max_x,fminf(x,max_x))

struct Force
{
        Position speed;
        Position acceleration;

        float maxSpeed;

        Force(Position _speed, Position _acceleration) :
        	speed(_speed),
        	acceleration(_acceleration) {}
        Force(float _a) :
        	speed(Position(_a)),
        	acceleration(Position(_a)) {}
	Force() :
		speed(Position(0.0f)),
		acceleration(Position(0.0f)) {}
};


enum EntityActivity
{
	ACTIVE,
	INACTIVE
};

struct Entity
{
        Position position = Position(0.0f);

        Force internal = Force(0.0f);
        Force external = Force(0.0f);

	EntityActivity active = INACTIVE;

        Entity (EntityType _entity_type, Position _position) :
		position(_position),
        	entity_type(_entity_type),
        	health(entity_property[_entity_type].defaultHealth),
        	active(ACTIVE)
        {

        }

        Entity (EntityType _entity_type, Position _position, Force _external) :
        	position(_position),
        	entity_type(_entity_type),
        	external(_external),
        	active(ACTIVE)
	{

	}

	Entity () {}

	float width, height;
	int uuid, health;
	EntityType entity_type;
	Item inventory[9][4];
};

 struct Camera
{
        Entity* entity;
        Position position;

        Camera(Entity* _entity) : entity(_entity), position(Position(0.0f)) {}
        Camera(Position _position) : position(_position) {}
};

struct EntityControlSystem
{
	int amount = 0;
	Entity entity[MAX_ENTITY_POOL];
	EntityControlSystem() {}

	int ECS_Find_First_Entity_Slot(EntityActivity activity)
	{
		for (int i = 0; i < MAX_ENTITY_POOL; i++)
			if (entity[i].active == activity)
			{
				return i;
			}
		return -1;
	}

	int ECS_Find_Next_Entity_Slot(EntityActivity activity, int current_index)
	{
		current_index++;
		for (int i = current_index; i < MAX_ENTITY_POOL + current_index; i++)
			if (entity[i%MAX_ENTITY_POOL].active == activity)
			{
				return i%256;
			}
		return -1;
	}

	Entity* ECS_Spawn(Entity _entity)
	{
		int index = ECS_Find_First_Entity_Slot(INACTIVE);
		//printf("Index: %d, Type: %d\n", index, entity.entity_type);
		if (index >= 0)
		{
			entity[index] = _entity;
			amount++;
			return &entity[index];
		} else return 0;
	}

	void ECS_Despawn(int index)
	{
		entity[index].active = INACTIVE;
		if(index == amount) amount--;
	}

	void ECS_Deactivate(int ACTIVE_ECS_index)
	{
		int free_slot_index = ECS_Find_First_Entity_Slot(INACTIVE);
		if (free_slot_index >= 0)
		{
			entity[free_slot_index] = entity[ACTIVE_ECS_index];
			if (free_slot_index == amount) amount++;
		}
		ECS_Despawn(ACTIVE_ECS_index);
	}

	void ECS_Tick()
	{
		for (int i = 0; i < MAX_ENTITY_POOL; i++)
		{
			if (entity[i].active == ACTIVE)
			{
				entity[i].external.speed.x += entity[i].external.acceleration.x;
				//FLOAT_CLAMP(entity[i].external.speed.x, entity[i].external.maxSpeed);
				entity[i].external.speed.y += entity[i].external.acceleration.y;
				//FLOAT_CLAMP(entity[i].external.speed.y, entity[i].external.maxSpeed);

				entity[i].internal.speed.x *= 0.50;
				entity[i].internal.speed.y *= 0.50;

				entity[i].internal.speed.x += entity[i].internal.acceleration.x;
				//FLOAT_CLAMP(entity[i].internal.speed.x, entity[i].internal.maxSpeed);
				entity[i].internal.speed.y += entity[i].internal.acceleration.y;
				//FLOAT_CLAMP(entity[i].internal.speed.y, entity[i].internal.maxSpeed);

				Position totalSpeed;

				totalSpeed.x = entity[i].external.speed.x+entity[i].internal.speed.x;
				totalSpeed.y = entity[i].external.speed.y+entity[i].internal.speed.y;

				entity[i].position.x += totalSpeed.x;
				entity[i].position.y += totalSpeed.y;
			}
		}
	}
};

static EntityControlSystem ACTIVE_ECS;
static EntityControlSystem INACTIVE_ECS;

void addItemToInventory(Entity *entity, Item item)
{
	int firstEmptyI = -1;
	int firstEmptyJ = -1;
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(entity->inventory[i][j].id == 0 && firstEmptyI < 0)
			{
				firstEmptyI = i;
				firstEmptyJ = j;
				entity->inventory[i][j].amount = 0;
			}
			if(entity->inventory[i][j].id == item.id)
				if(entity->inventory[i][j].amount < 999)
					entity->inventory[i][j].amount += item.amount;
		}
	}
}

void playerItemInit(Entity *player)
{
	//Entity player = initEntity(PLAYER, player);
	//Give the player lots of items to debug with
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			player->inventory[i][j].id = 0;
			player->inventory[i][j].amount = 0;
			if(j == 0)
			{
				player->inventory[i][j].id = i+1;
				player->inventory[i][j].amount = 16;
			}
		}
	}
}

#endif // ENTITY_H_INCLUDED
