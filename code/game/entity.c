#include "../common.h"

#include "../system/draw.h"
#include "../system/atlas.h"
#include "../system/utils.h"
#include "../system/memory.h"

#include"dungeon.h"

#include "collision.h"
#include "entity.h"
#include "player.h"

extern App app;
extern Dungeon dungeon;

static uint32_t * entities;
static uint32_t   numEntities[MAX_NUM_THREADS];

void resetEntityUpdates(void)
{
    memset(numEntities, 0, sizeof(uint32_t) * MAX_NUM_THREADS);
}

bool initEntities(void)
{
    entities = allocateTransientMemory(MAX_NUM_ENTITIES * sizeof(uint32_t));
    if(entities == NULL)
    {
        return false;
    }

    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        numEntities[i] = 0;   
    }
 
    return true;
}

void addEntityToUpdateList(uint32_t index, uint32_t thread)
{
    uint32_t * base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    base[numEntities[thread]] = index;
    numEntities[thread]++;
    SDL_assert(numEntities[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
}

void moveEntity(Entity * entity, float dx, float dy)
{
    float newX, newY;
    Actor * actor = (Actor *)entity->data;
    SDL_assert(actor !=  NULL);
    float vel = actor->velocity;

    newX = entity->p.x + dx * vel;
    newY = entity->p.y + dy * vel;

    if(dx > 0)
    {
        entity->facing = FACING_RIGHT;
    }
    else if (dx < 0)
    {
        entity->facing = FACING_LEFT;
    }

    bool move = true;

    Rect entityRect = {newX, newY, entity->width, entity->width};

    MapTile* playerTiles[8];
    playerTiles[0] = getTileAtRowColLayerRaw((int)newY - 1, (int)newX - 1, 1);
    playerTiles[1] = getTileAtRowColLayerRaw((int)newY - 1, (int)newX, 1);
    playerTiles[2] = getTileAtRowColLayerRaw((int)newY - 1, (int)newX + 1, 1);
    playerTiles[3] = getTileAtRowColLayerRaw((int)newY, (int)newX - 1, 1);
    playerTiles[4] = getTileAtRowColLayerRaw((int)newY, (int)newX + 1, 1);
    playerTiles[5] = getTileAtRowColLayerRaw((int)newY + 1, (int)newX - 1, 1);
    playerTiles[6] = getTileAtRowColLayerRaw((int)newY + 1, (int)newX, 1);
    playerTiles[7] = getTileAtRowColLayerRaw((int)newY + 1, (int)newX + 1, 1);

    for(int i = 0; i < 8; i++)
    {
        MapTile * tile = playerTiles[i];

        Rect tileRect = {tile->p.x, tile->p.y, 0.75f, 0.75f};

        if((tile->tile == TILE_WALL) && rectangleRectangleCollision(tileRect, entityRect))
        {
            move = false;
        }
    }

    if((newX >= 0) && (newX < MAP_WIDTH) && (newY >= 0) && (newY < MAP_HEIGHT) && (move == true))
    {
        for(int i = 0; i < MAX_NUM_THREADS; i++)
        {
            uint32_t * base = entities + i * MAX_ENTITY_COUNT_PER_THREAD;

            for(int j = 0; j < numEntities[i]; j++)
            {
                uint32_t index = base[j];
                Entity * collider = &dungeon.entities[index];
                if (entity != collider)
                {
                    Vec2f newPos = {newX + entity->width/2.0f, newY + entity->width/2.0f};
                    Actor * colliderActor = (Actor *)collider->data;
                    if (colliderActor && 
                        (circleCircleCollision(newPos, entity->width / 2.0f, collider->p, collider->width/2.0f) == true) && 
                        colliderActor->health > 0)
                    {
                        move = false;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        move = false;
    }

    if(move == true)
    {
        entity->p.x = newX;
        entity->p.y = newY;
    }
}

static inline Entity * getEntityById(int id)
{
    return &dungeon.entities[id];
}

bool isEntityAlive(Entity * entity)
{
    bool alive = BIT_CHECK(entity->flags, ENTITY_IS_ALIVE_BIT);
    return alive;
}

static void updateBarrel(Entity * entity)
{

}

static inline uint32_t getEntityIndex(uint32_t i, uint32_t thread)
{
    uint32_t * base = entities + i * MAX_ENTITY_COUNT_PER_THREAD;
    return base[i];
}

void updateEntities(void)
{
    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        for(int j = 0; j < numEntities[i]; j++)
        {
            uint32_t index = getEntityIndex(j, i);
            Entity * entity = &dungeon.entities[index];
            
            switch(entity->entityType)
            {
                case ET_PLAYER:
                    updatePlayer(entity);
                    break;
                case ET_BARREL:
                    updateBarrel(entity);
                    break;
                default:
                    break;
            }
        }
    }
}