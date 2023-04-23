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

bool checkEntityCollisions(Entity *entity, Vec2f *newPos)
{
    bool move = true;
    if((newPos->x >= 0) && (newPos->x < MAP_WIDTH) && (newPos->y >= 0) && (newPos->y < MAP_HEIGHT))
    {
        for(int i = 0; i < totalNumEntities; i++)
        {
            uint32_t index = entities[i];
            Entity * collider = &dungeon.entities[index];
            if (entity != collider)
            {
                Actor * colliderActor = (Actor *)collider->data;
                if (colliderActor && 
                    canEntityCollide(collider))
                {
                    resolveCircleCircleCollision(newPos, entity->width / 2.0f, &collider->p, collider->width / 2.0f);
                }
            }
        }
    }
    else
    {
        move = false;
    }

    return move;
}

bool moveEntityRaw(Entity *e, float dx, float dy)
{
    bool result = false;

    Actor * actor = (Actor *)e->data;
    float vel = actor->velocity;

    Vec2f newPos;
    newPos.x = e->p.x + dx * vel;
    newPos.y = e->p.y + dy * vel;
    Vec2f finalPos = newPos;

    bool move = checkEntityCollisions(e, &finalPos);
    if (move)
    {
        if ((finalPos.x != newPos.x) || (finalPos.y != newPos.y))
        {
            //collision happened
            result = true;
        }

        actor->dP.x = dx * vel;
        actor->dP.y = dy * vel;

        e->p.x = finalPos.x;
        e->p.y = finalPos.y;
    }
    debugDrawRect(e->p, e->width, e->height, 255, 0, 0, 0);
    return result;
}

void moveEntity(Entity * entity, float dx, float dy)
{
    bool move = true;

    Actor * actor = (Actor *)entity->data;
    float vel = actor->velocity;
    actor->dP.x = 0.0f;
    actor->dP.y = 0.0f;

    Vec2f newPos;
    newPos.x = entity->p.x + dx * vel;
    newPos.y = entity->p.y + dy * vel;
    
    Rect entityRect = {newPos.x, newPos.y, entity->width, entity->width};
    move = checkTileCollisions(entityRect);
    if (move == true)
    {
        move = checkEntityCollisions(entity, &newPos);
        if(move == true)
        {
            actor->dP.x = dx * vel;
            actor->dP.y = dy * vel;
            
            entity->p.x = newPos.x;
            entity->p.y = newPos.y;
        }
    }
   
    debugDrawRect(entity->p, entity->width, entity->height, 255, 0, 0, 0);
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

bool canEntityCollide(Entity *entity)
{
    bool canCollide = BIT_CHECK(entity->flags, ENTITY_IS_ALIVE_BIT) && 
                      BIT_CHECK(entity->flags, ENTITY_CAN_COLLIDE_BIT);
    return canCollide;
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
