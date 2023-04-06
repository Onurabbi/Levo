#include "../common.h"

#include "../system/atlas.h"
#include "../system/animation.h"
#include "../system/memory.h"

#include "entityFactory.h"
#include "entity.h"
#include "actor.h"
#include "weapon.h"

enum
{
    ENTITY_QUEUE,
    ACTOR_QUEUE,
    WEAPON_QUEUE,
};

typedef struct 
{
    uint32_t *queue;
    uint32_t readOffset;
    uint32_t writeOffset;
    uint32_t count;
    uint32_t maxCount;
} IndexQueue;

extern Dungeon dungeon;

static InitFunc initFuncs[MAX_NUM_INIT_FUNCS];
static uint32_t numInitFuncs;

static IndexQueue entityQueue;
static IndexQueue actorQueue;
static IndexQueue weaponQueue;

static inline bool queueIsEmpty(IndexQueue *indexQueue)
{
    return (indexQueue->count == 0);
}

static inline bool queueIsFull(IndexQueue *indexQueue)
{
    return (indexQueue->count == indexQueue->maxCount);
}

static uint32_t popIndexQueue(IndexQueue *indexQueue)
{
    uint32_t result;
    result = indexQueue->queue[indexQueue->readOffset];
    indexQueue->readOffset = (indexQueue->readOffset + 1) % indexQueue->maxCount;
    indexQueue->count--;
    return result;
}

static void pushIndexToQueue(IndexQueue *indexQueue, uint32_t index)
{
    indexQueue->queue[indexQueue->writeOffset] = index;
    indexQueue->writeOffset = (indexQueue->writeOffset + 1) % indexQueue->maxCount;
    indexQueue->count++;
}

static char * entityTypes[ET_MAX] =
{
    "Player",
    "Barrel",
    "Projectile",
    "Longsword"
};

static EntityType getEntityType(char * entityType)
{
    for(int i = 0; i < ET_MAX; i++)
    {
        if(strcmp(entityType, entityTypes[i]) == 0)
        {
            return (EntityType)i;
        }
    }
    return ET_MAX;
}

static InitFunc * getInitFunction(EntityType entityType)
{
    return &initFuncs[entityType];
}

static uint32_t getNewIndex(IndexQueue *queue, int type)
{
    uint32_t result;

    if(queueIsEmpty(queue) == false)
    {
        result = popIndexQueue(queue);
    }
    else
    {
        switch(type)
        {
            case ENTITY_QUEUE:
                result = dungeon.numEntities++;
                break;
            case ACTOR_QUEUE:
                result = dungeon.numActors++;
                break;
            case WEAPON_QUEUE:
                result = dungeon.numWeapons++;
                break;
            default:
                break;
        }
    }
}

static void initProjectile(Entity * projectile)
{
    printf("initialising projectile");
    
    projectile->angle = 0.0;
    projectile->width = 0.375f;
    projectile->height = 0.50f;
    projectile->entitySprites.sprites[0] = getSprite("gfx/entities/knife1.png");
    projectile->entitySprites.drawableCount = 1;

    projectile->flags = 0;

    BIT_SET(projectile->flags, ENTITY_CAN_UPDATE_BIT);
    BIT_SET(projectile->flags, ENTITY_CAN_COLLIDE_BIT);
    BIT_SET(projectile->flags, ENTITY_IS_ALIVE_BIT);
    BIT_SET(projectile->flags, ENTITY_IS_VISIBLE_BIT);
}

static void initLongsword(Entity * longSword)
{
    longSword->angle = 0.0;
    longSword->width = 0.0f;
    longSword->height = 0.0f;
    longSword->flags = 0;
    //weapon sprite is only visible until it's picked up. we can (and probably should) actually use some entirely different sprite here. 
    longSword->entitySprites.sprites[0] = getSprite("gfx/entities/longswordLeftIdle1.png");
    longSword->entitySprites.drawableCount = 1;

    BIT_SET(longSword->flags, ENTITY_CAN_UPDATE_BIT);
    BIT_SET(longSword->flags, ENTITY_CAN_COLLIDE_BIT);
    BIT_SET(longSword->flags, ENTITY_IS_ALIVE_BIT);
    BIT_SET(longSword->flags, ENTITY_IS_VISIBLE_BIT);

    uint32_t longSwordIndex = getNewIndex(&weaponQueue, WEAPON_QUEUE);

    Weapon *longSwordWeapon = &dungeon.weapons[longSwordIndex];
    longSwordWeapon->weaponIndex = longSwordIndex;
    longSwordWeapon->owner = longSword;
}

static void initPlayer(Entity * player)
{
    player->angle = 0.0;
    player->width = 0.7f;
    player->height = 0.50f;
    player->flags = 0;
    
    player->entitySprites.sprites[HEAD] = getSprite("gfx/entities/heroineLeftIdle1.png");
    player->entitySprites.sprites[BODY] = getSprite("gfx/entities/headLongLeftIdle1.png");
    
    player->entitySprites.drawableCount = 2;

    BIT_SET(player->flags, ENTITY_CAN_UPDATE_BIT);
    BIT_SET(player->flags, ENTITY_CAN_COLLIDE_BIT);
    BIT_SET(player->flags, ENTITY_IS_ALIVE_BIT);
    BIT_SET(player->flags, ENTITY_IS_VISIBLE_BIT);

    uint32_t actorIndex = getNewIndex(&actorQueue, ACTOR_QUEUE);

    Actor * playerActor = &dungeon.actors[actorIndex];
    playerActor->actorIndex = actorIndex;
    playerActor->owner = player;
    playerActor->animGroup = getAnimationGroup("gfx/animations/heroine.animGroup");
    playerActor->dP.x = 0;
    playerActor->dP.y = 0;
    playerActor->facing = FACING_LEFT;
    playerActor->attacking = false;
    playerActor->switchAnim = false;
    playerActor->health = 10;
    playerActor->velocity = 0.15f;
    player->data = playerActor;
}

static void initBarrel(Entity * barrel)
{
    barrel->angle = 0.0;
    barrel->width = 0.7f;
    barrel->height = 0.50f;
    barrel->entitySprites.sprites[0] = getSprite("gfx/entities/barrel.png");
    barrel->entitySprites.drawableCount = 1;
    
    barrel->flags = 0;
    BIT_SET(barrel->flags, ENTITY_CAN_UPDATE_BIT);
    BIT_SET(barrel->flags, ENTITY_CAN_COLLIDE_BIT);
    BIT_SET(barrel->flags, ENTITY_IS_ALIVE_BIT);
    BIT_SET(barrel->flags, ENTITY_IS_VISIBLE_BIT);

    uint32_t actorIndex = getNewIndex(&actorQueue, ACTOR_QUEUE);
    Actor * barrelActor = &dungeon.actors[actorIndex];
    barrelActor->actorIndex = actorIndex;
    barrelActor->owner = barrel;
    barrelActor->health = 10;
    barrelActor->velocity = 0;

    barrel->data = barrelActor;
}

Entity* initEntity(char * name)
{
    SDL_assert(name);

    EntityType type = getEntityType(name);
    Entity * result;

    uint32_t entityIndex = getNewIndex(&entityQueue, ENTITY_QUEUE);
    result = &dungeon.entities[entityIndex];
    memset(result, 0, sizeof(Entity));
    result->entityIndex = entityIndex;

    result->id = dungeon.entityId++;
    result->entityType = type;

    getInitFunction(type)->init(result);
    return result;
}

void removeActor(Actor *a)
{
    if(queueIsFull(&actorQueue) == false)
    {
        printf("pushing actor %d to queue!\n", a->actorIndex);
        pushIndexToQueue(&actorQueue, a->actorIndex);
    }   
}

void removeEntity(Entity * e)
{
    if(queueIsFull(&entityQueue) == false)
    {
        printf("pushing entity %d to queue!\n", e->entityIndex);
        pushIndexToQueue(&entityQueue, e->entityIndex);
    }
    e->flags = 0;
}

static void addInitFunc(char * name, void (*init)(Entity *e))
{
    InitFunc * initFunc = getInitFunction(getEntityType(name));

	STRCPY(initFunc->name, name);
	initFunc->init = init;
    numInitFuncs++;

    SDL_assert(numInitFuncs < MAX_NUM_INIT_FUNCS);
}

static bool initQueue(IndexQueue *queue, uint32_t maxCount)
{
    queue->queue = allocatePermanentMemory(sizeof(uint32_t) * maxCount);
    if (queue->queue == NULL)
    {
        return false;
    }
    queue->count = 0;
    queue->readOffset = 0;
    queue->writeOffset = 0;
    queue->maxCount = maxCount;
    return true;
}

bool initEntityFactory(void)
{
    memset(initFuncs, 0, sizeof(initFuncs));
    numInitFuncs = 0;

    if(initQueue(&entityQueue, MAX_NUM_ENTITIES) == false)
    {
        //TODO: LOGGING
        return false;
    }

    if(initQueue(&actorQueue, MAX_NUM_ACTORS) == false)
    {
        //TODO: LOGGING
        return false;
    }

    if(initQueue(&weaponQueue, MAX_NUM_WEAPONS) == false)
    {
        //TODO: LOGGING
        return false;
    }

    addInitFunc("Player", initPlayer);
    addInitFunc("Barrel", initBarrel);
    addInitFunc("Projectile", initProjectile);
    addInitFunc("Longsword", initLongsword);

    return true;
}