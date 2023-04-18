#include "../common.h"

#include "../system/draw.h"
#include "../system/atlas.h"
#include "../system/utils.h"
#include "../system/memory.h"
#include "../system/vector.h"
#include "../system/input.h"

#include "entityFactory.h"
#include "dungeon.h"

#include "collision.h"
#include "entity.h"
#include "player.h"
#include "weapon.h"
#include "enemy.h"

extern App app;
extern Dungeon dungeon;

static uint32_t *entities;
static uint32_t *numEntities;
static uint32_t  totalNumEntities;

void die(Entity *e)
{
    switch(e->entityType)
    {
        case ET_ENEMY:
        case ET_PLAYER:
            Actor * actor = (Actor *)e->data;
            removeActor(actor);
            break;
        default:
            break;
    }
    removeEntity(e);
}

SimulationRegion getSimulationRegion(void)
{
    SimulationRegion result;
    result.entities = entities;
    result.totalNumEntities = totalNumEntities;
    return result;
}

void denselyPackEntities(void)
{
    uint32_t *dest = entities + numEntities[0];
    totalNumEntities = numEntities[0];

    for(int i = 0; i < MAX_NUM_THREADS - 1; i++)
    {
        uint32_t *src = entities + (i + 1) * MAX_ENTITY_COUNT_PER_THREAD;
        uint32_t bytes = numEntities[i+1] * sizeof(uint32_t);

        memcpy(dest, src, bytes);

        dest += numEntities[i+1];
        totalNumEntities += numEntities[i+1];
    }
}

void resetEntityUpdates(void)
{
    for(int thread = 0; thread < MAX_NUM_THREADS; thread++)
    {
        numEntities[thread] = 0;
    }
    totalNumEntities = 0;
}

static inline uint32_t getEntityIndex(uint32_t i, uint32_t thread)
{
    uint32_t * base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    return base[i];
}

static inline void setEntityIndex(uint32_t i, uint32_t thread)
{
    uint32_t *base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    base[numEntities[thread]] = i;
}

bool initEntities(void)
{
    entities = allocatePermanentMemory(MAX_NUM_ENTITIES * sizeof(uint32_t));
    if(entities == NULL)
    {
        return false;
    }

    numEntities = allocatePermanentMemory(MAX_NUM_ENTITIES * sizeof(uint32_t));
    if(numEntities == NULL)
    {
        return false;
    }
 
    return true;
}

void addEntityToUpdateList(uint32_t index, uint32_t thread)
{
    setEntityIndex(index, thread);
    numEntities[thread]++;
    SDL_assert(numEntities[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
}

void  moveEntityRaw(Entity *e, float dx, float dy)
{
    e->p.x += dx;
    e->p.y += dy;
}

void moveEntity(Entity * entity, float dx, float dy)
{
    float newX, newY;

    Actor * actor = (Actor *)entity->data;
    float vel = actor->velocity;
    actor->dP.x = 0.0f;
    actor->dP.y = 0.0f;
    
    newX = entity->p.x + dx * vel;
    newY = entity->p.y + dy * vel;

    bool move = true;

    Rect entityRect = {newX, newY, entity->width, entity->width};
    debugDrawRect(entity->p, entity->width, entity->height, 255, 0, 0, 0);
    move = checkTileCollisions(entityRect);
    if((newX >= 0) && (newX < MAP_WIDTH) && (newY >= 0) && (newY < MAP_HEIGHT) && (move == true))
    {
        for(int i = 0; i < totalNumEntities; i++)
        {
            uint32_t index = entities[i];
            Entity * collider = &dungeon.entities[index];
            if (entity != collider)
            {
                Vec2f newPos = {newX, newY};
                Actor * colliderActor = (Actor *)collider->data;
                if (colliderActor && 
                    (circleCircleCollision(newPos, entity->width / 2.0f, collider->p, collider->width/2.0f) == true) && 
                     isEntityAlive(collider))
                {
                    move = false;
                    break;
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

        //entity steps on new tile
        MapTile *newTile = getTileAtRowColLayerRaw(entity->p.y, entity->p.x, 1);
        if (newTile != entity->currentTile)
        {
            entity->currentTile->flags = 0x0;
            entity->currentTile = newTile;
            entity->currentTile->flags = 0x1;
        }
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

bool isEntityVisible(Entity *entity)
{
    bool visible = BIT_CHECK(entity->flags, ENTITY_IS_VISIBLE_BIT);
    return visible;
}

static void updateBarrel(Entity * entity)
{

}

void updateEntities(void)
{
    for(int i = 0; i < totalNumEntities; i++)
    {
        uint32_t index = entities[i];
        Entity *entity = &dungeon.entities[index];
        switch(entity->entityType)
        {
            case ET_PLAYER:
                updatePlayer(entity);
                break;
            case ET_LONGSWORD:
                updateWeapon(entity);
                break;
            case ET_ENEMY:
                updateEnemy(entity);
                break;
            default:
                break;
        }
    }
    clearInput(&app.input);
}
