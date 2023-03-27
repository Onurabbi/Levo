#include "../common.h"

#include "../system/draw.h"
#include "../system/atlas.h"
#include "../system/utils.h"
#include "../system/memory.h"

#include "collision.h"
#include "entity.h"
#include "player.h"

extern App app;
extern Dungeon dungeon;

static uint32_t * entities[MAX_NUM_THREADS];
static uint32_t   numEntities[MAX_NUM_THREADS];

void resetEntityUpdates(void)
{
    memset(numEntities, 0, sizeof(uint32_t) * MAX_NUM_THREADS);
}

bool initEntities(void)
{
    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        entities[i] = allocateTransientMemory(MAX_ENTITY_COUNT_PER_THREAD * sizeof(uint32_t));
        numEntities[i] = 0;
        if(entities[i] == NULL)
        {
            return false;
        }
    }
    return true;
}

void addEntityToUpdateList(uint32_t index, uint32_t thread)
{
    entities[thread][numEntities[thread]] = index;
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

    if((newX >= 0) && (newX < MAP_WIDTH) && (newY >= 0) && (newY < MAP_HEIGHT))
    {
        for(int i = 0; i < MAX_NUM_THREADS; i++)
        {
            for(int j = 0; j < numEntities[i]; j++)
            {
                uint32_t index = entities[i][j];
                Entity * collider = &dungeon.entities[index];
                if (entity != collider)
                {
                    Vec2f newPos = {newX, newY};
                    Actor * colliderActor = (Actor *)collider->data;
                    if(colliderActor && 
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

void updateEntities(void)
{
    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        printf("updating %d entities\n", numEntities[i]);
        for(int j = 0; j < numEntities[i]; j++)
        {
            uint32_t index = entities[i][j];
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