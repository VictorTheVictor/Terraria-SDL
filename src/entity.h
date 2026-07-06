#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#define MAX_ENTITY_POOL 256

#include <stdbool.h>
#include <stdio.h>
#include "item.h"
#include "entity_type.h"

typedef struct Entity
{
	float x, y, dx, dy, ddx, ddy, max_dx, max_dy, width, height;
	int uuid, health;
	EntityType entity_type;
	bool active;
	Item inventory[9][4];
} Entity;

typedef struct
{
	int amount;
	Entity entity[MAX_ENTITY_POOL];
} EntityControlSystem;

static EntityControlSystem ACTIVE_ECS;
static EntityControlSystem INACTIVE_ECS;

int ECS_Find_Free_Entity_Slot(EntityControlSystem* entity_control_system)
{
	int result = -1;
	for (int i = 0; i < MAX_ENTITY_POOL; i++)
		if (entity_control_system->entity[i].active == 0)
		{
			result = i;
			break;
		}
}

Entity spawnRegularEntity(int x, int y, EntityType entity_type)
{
	Entity entity;
	entity.x = x;
	entity.y = y;
	entity.entity_type = entity_type;
	entity.health = entity_property[entity_type].defaultHealth;
	entity.active = 1;
	return entity;
}

Entity spawnProjectileEntity(int x, int y, int dx, int dy)
{
	Entity projectile;
	projectile.x = x;
	projectile.y = y;
	projectile.dx = dx;
	projectile.dy = dy;
	projectile.active = 1;
	return projectile;
}

Entity* ECS_Spawn(Entity entity)
{
	int index = ECS_Find_Free_Entity_Slot(&ACTIVE_ECS);
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
	int free_slot_index = ECS_Find_Free_Entity_Slot(&INACTIVE_ECS);
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
			ACTIVE_ECS.entity[i].dx += ACTIVE_ECS.entity[i].ddx;
			ACTIVE_ECS.entity[i].dy += ACTIVE_ECS.entity[i].ddy;
			if(ACTIVE_ECS.entity[i].dx > ACTIVE_ECS.entity[i].max_dx) ACTIVE_ECS.entity[i].dx = ACTIVE_ECS.entity[i].max_dx;
			if(ACTIVE_ECS.entity[i].dy > ACTIVE_ECS.entity[i].max_dy) ACTIVE_ECS.entity[i].dy = ACTIVE_ECS.entity[i].max_dy;
			if(ACTIVE_ECS.entity[i].dx < -ACTIVE_ECS.entity[i].max_dx) ACTIVE_ECS.entity[i].dx = -ACTIVE_ECS.entity[i].max_dx;
			if(ACTIVE_ECS.entity[i].dy < -ACTIVE_ECS.entity[i].max_dy) ACTIVE_ECS.entity[i].dy = -ACTIVE_ECS.entity[i].max_dy;
			ACTIVE_ECS.entity[i].x += ACTIVE_ECS.entity[i].dx;
			ACTIVE_ECS.entity[i].y += ACTIVE_ECS.entity[i].dy;
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
