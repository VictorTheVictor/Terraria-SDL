#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#define MAX_ENTITY_POOL 256

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "item.h"
#include "entity_type.h"

#define POS_ZERO ((Position){0.0f,0.0f})
#define POS_XY(x) ((Position){(float)(x),(float)(x)})
#define POS_SET(x,y) ((Position){(float)(x),(float)(y)})

#define FLOAT_CLAMP(x,max_x) (x)=fmaxf(-max_x,fminf(x,max_x))

#define FORCE_ZERO ((Force){0.0f,0.0f,0.0f})

typedef struct
{
        float x, y;
} Position;

typedef struct
{
        Position speed;
        Position acceleration;
        float maxSpeed;
} Force;

typedef struct Entity
{
        Position position;

        Force internal;
        Force external;

        //TODO: make this its own struct and have one for for self movement, one for force movement
	float width, height;
	int uuid, health;
	EntityType entity_type;
	bool active;
	Item inventory[9][4];
} Entity;

typedef struct
{
        Entity* entity;
        Position position;
} Camera;

Camera spawnCamera(Entity* _entity)
{
        Camera camera = {_entity, POS_ZERO};
        return camera;
}

typedef struct
{
	int amount;
	Entity entity[MAX_ENTITY_POOL];
} EntityControlSystem;

static EntityControlSystem ACTIVE_ECS;
static EntityControlSystem INACTIVE_ECS;

int ECS_Find_First_Entity_Slot(EntityControlSystem* entity_control_system, bool activity)
{
	for (int i = 0; i < MAX_ENTITY_POOL; i++)
		if (entity_control_system->entity[i].active == activity)
		{
			return i;
		}
	return -1;
}

int ECS_Find_Next_Occupied_Entity_Slot(EntityControlSystem* entity_control_system, bool activity, int current_index)
{
	current_index++;
	for (int i = current_index; i < MAX_ENTITY_POOL + current_index; i++)
		if (entity_control_system->entity[i%MAX_ENTITY_POOL].active == activity)
		{
			return i%256;
		}
	return -1;
}

Entity spawnRegularEntity(Position _position, EntityType _entity_type)
{
	Entity entity;
	entity.position = _position;
	entity.internal = FORCE_ZERO;
	entity.external = FORCE_ZERO;
	entity.entity_type = _entity_type;
	entity.health = entity_property[_entity_type].defaultHealth;
	entity.active = 1;
	return entity;
}

/*Entity spawnProjectileEntity(Position _position, Position _speed)
{
	Entity projectile;
	projectile.position = _position;
	projectile.speed = _speed;
	projectile.active = 1;
	return projectile;
}*/

Entity* ECS_Spawn(Entity entity)
{
	int index = ECS_Find_First_Entity_Slot(&ACTIVE_ECS, 0);
	printf("Index: %d, Type: %d\n", index, entity.entity_type);
	if (index >= 0)
	{
		ACTIVE_ECS.entity[index] = entity;
		ACTIVE_ECS.amount++;
		return &ACTIVE_ECS.entity[index];
	} else return 0;
}

void ECS_Despawn(int index)
{
	ACTIVE_ECS.entity[index].active = 0;
	if(index == ACTIVE_ECS.amount) ACTIVE_ECS.amount--;
}

void ECS_Deactivate(int ACTIVE_ECS_index)
{
	int free_slot_index = ECS_Find_First_Entity_Slot(&INACTIVE_ECS, 0);
	if (free_slot_index >= 0)
	{
		INACTIVE_ECS.entity[free_slot_index] = INACTIVE_ECS.entity[ACTIVE_ECS_index];
		if (free_slot_index == INACTIVE_ECS.amount) INACTIVE_ECS.amount++;
	}
	ECS_Despawn(ACTIVE_ECS_index);
}

void ECS_Tick()
{
	for (int i = 0; i < MAX_ENTITY_POOL; i++)
	{
		if (ACTIVE_ECS.entity[i].active)
		{
			ACTIVE_ECS.entity[i].external.speed.x += ACTIVE_ECS.entity[i].external.acceleration.x;
			//FLOAT_CLAMP(ACTIVE_ECS.entity[i].external.speed.x, ACTIVE_ECS.entity[i].external.maxSpeed);
			ACTIVE_ECS.entity[i].external.speed.y += ACTIVE_ECS.entity[i].external.acceleration.y;
			//FLOAT_CLAMP(ACTIVE_ECS.entity[i].external.speed.y, ACTIVE_ECS.entity[i].external.maxSpeed);

			ACTIVE_ECS.entity[i].internal.speed.x *= 0.50;
			ACTIVE_ECS.entity[i].internal.speed.y *= 0.50;

			ACTIVE_ECS.entity[i].internal.speed.x += ACTIVE_ECS.entity[i].internal.acceleration.x;
			//FLOAT_CLAMP(ACTIVE_ECS.entity[i].internal.speed.x, ACTIVE_ECS.entity[i].internal.maxSpeed);
			ACTIVE_ECS.entity[i].internal.speed.y += ACTIVE_ECS.entity[i].internal.acceleration.y;
			//FLOAT_CLAMP(ACTIVE_ECS.entity[i].internal.speed.y, ACTIVE_ECS.entity[i].internal.maxSpeed);

			Position totalSpeed;

			totalSpeed.x = ACTIVE_ECS.entity[i].external.speed.x+ACTIVE_ECS.entity[i].internal.speed.x;
			totalSpeed.y = ACTIVE_ECS.entity[i].external.speed.y+ACTIVE_ECS.entity[i].internal.speed.y;

			ACTIVE_ECS.entity[i].position.x += totalSpeed.x;
			ACTIVE_ECS.entity[i].position.y += totalSpeed.y;
		}
	}
}


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
};

#endif // ENTITY_H_INCLUDED
