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

static inline uint32_t getEntityIndex(uint32_t i, uint32_t thread)
{
    uint32_t * base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    return base[i];
}

static inline void setEntityIndex(uint32_t i, uint32_t thread)
{
    uint32_t * base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    base[numEntities[thread]] = i;
}

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
    setEntityIndex(index, thread);
    numEntities[thread]++;
    SDL_assert(numEntities[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
}

void moveEntity(Entity * entity, float dx, float dy)
{
    float newX, newY;

    Actor * actor = (Actor *)entity->data;
    float vel = actor->velocity;
    actor->dP.x = 0.0f;
    actor->dP.y = 0.0f;
    
    if(dx > 0)
    {
        actor->facing = FACING_RIGHT;
    }
    else if (dx < 0)
    {
        actor->facing = FACING_LEFT;
    }

    newX = entity->p.x + dx * vel;
    newY = entity->p.y + dy * vel;

    bool move = true;

    Rect entityRect = {newX - entity->width/2.0f, newY - entity->width/2.0f, entity->width, entity->width};

    MapTile * tiles[9];

    tiles[0]  = getTileAtRowColLayerRaw((int)(newY), (int)(newX), 1);
    tiles[1]  = getTileAtRowColLayerRaw((int)(newY - 1), (int)(newX - 1), 1);
    tiles[2]  = getTileAtRowColLayerRaw((int)(newY - 1), (int)(newX), 1);
    tiles[3]  = getTileAtRowColLayerRaw((int)(newY - 1), (int)(newX + 1), 1);
    tiles[4]  = getTileAtRowColLayerRaw((int)(newY), (int)(newX - 1), 1);
    tiles[5]  = getTileAtRowColLayerRaw((int)(newY), (int)(newX + 1), 1);
    tiles[6]  = getTileAtRowColLayerRaw((int)(newY + 1), (int)(newX - 1), 1);
    tiles[7]  = getTileAtRowColLayerRaw((int)(newY + 1), (int)(newX), 1);
    tiles[8]  = getTileAtRowColLayerRaw((int)(newY + 1), (int)(newX + 1), 1);

    for(int i = 0; i < 9; i++)
    {
        Rect tileRect = {tiles[i]->p.x, tiles[i]->p.y, 0.75f, 0.375f};
        if((tiles[i]->tile == TILE_WALL) && rectangleRectangleCollision(tileRect, entityRect))
        {
            move = false;
        }
    }

    if((newX >= 0) && (newX < MAP_WIDTH) && (newY >= 0) && (newY < MAP_HEIGHT) && (move == true))
    {
        for(int thread = 0; thread < MAX_NUM_THREADS; thread++)
        {
            for(int i = 0; i < numEntities[thread]; i++)
            {
                uint32_t index = getEntityIndex(i, thread);
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
        actor->dP.x = dx * vel;
        actor->dP.y = dy * vel;
        
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