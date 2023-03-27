#include "../common.h"

#include "../system/atlas.h"
#include "../system/animation.h"


#include "entityFactory.h"
#include "entity.h"
#include "actor.h"

extern Dungeon dungeon;

static InitFunc initFuncs[MAX_NUM_INIT_FUNCS];
static uint32_t numInitFuncs;

//to handle deleted entities
static uint32_t entityIndexQueue[MAX_NUM_ENTITIES];
static uint32_t readOffset;
static uint32_t writeOffset;
static uint32_t numIndices;

static char * entityTypes[ET_MAX] =
{
    "Player",
    "Barrel",
    "Projectile",
    "Tree1",
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

static void initProjectile(Entity * projectile)
{
    printf("initialising projectile");
    
    projectile->angle = 0.0;
    projectile->width = 0.375f;
    projectile->height = 0.50f;
    projectile->sprite = getSprite("gfx/entities/knife1.png");
    projectile->flags = 0;

    projectile->flags = BIT_SET(projectile->flags, ENTITY_CAN_UPDATE_BIT) | 
                        BIT_SET(projectile->flags, ENTITY_CAN_COLLIDE_BIT) |
                        BIT_SET(projectile->flags, ENTITY_IS_ALIVE_BIT);
                  
}

static void initPlayer(Entity * player)
{
    player->angle = 0.0;
    player->width = 0.375f;
    player->height = 0.50f;
    player->sprite = getSprite("gfx/entities/barrel.png");
    player->flags = 0;

    player->flags =  BIT_SET(player->flags, ENTITY_CAN_UPDATE_BIT) | 
                     BIT_SET(player->flags, ENTITY_CAN_COLLIDE_BIT) |
                     BIT_SET(player->flags, ENTITY_IS_ALIVE_BIT);


    Actor * playerActor = getNewActor();
    playerActor->health = 10;
    playerActor->velocity = 1.0f;
    player->data = playerActor;
}

static void initBarrel(Entity * barrel)
{
    barrel->angle = 0.0;
    barrel->width = 1.0f;
    barrel->height = 0.50f;
    barrel->sprite = getSprite("gfx/entities/barrel.png");
    barrel->flags = 0;
    barrel->flags =  BIT_SET(barrel->flags, ENTITY_CAN_UPDATE_BIT) | 
                     BIT_SET(barrel->flags, ENTITY_CAN_COLLIDE_BIT) |
                     BIT_SET(barrel->flags, ENTITY_IS_ALIVE_BIT);

    Actor * barrelActor = getNewActor();
    barrelActor->health = 1;
    barrelActor->velocity = 0;
    barrel->data = barrelActor;
}

static void initTree(Entity * tree)
{
    tree->angle = 0.0;
    tree->width = 0.75f;
    tree->height = 0.75f;
    tree->sprite = getSprite("gfx/entities/tree1.png");
    tree->data = NULL;
}

static uint32_t getNewEntityIndex(void)
{
    uint32_t result;
    if(numIndices > 0)
    {
        result = entityIndexQueue[readOffset];
        readOffset = (readOffset + 1) % MAX_NUM_ENTITIES;
        numIndices--;
    }
    else
    {
        SDL_assert(dungeon.numEntities < MAX_NUM_ENTITIES);
        result = dungeon.numEntities++;
    }
    return result;
}

Entity* initEntity(char * name)
{
    SDL_assert(name);

    EntityType type = getEntityType(name);
    Entity * result;

    uint32_t entityIndex = getNewEntityIndex();
    result = &dungeon.entities[entityIndex];
    result->entityIndex = entityIndex;

    result->id = dungeon.entityId++;
    result->entityType = type;

    getInitFunction(type)->init(result);
    return result;
}

static void queueRemovedEntityIndex(uint32_t entityIndex)
{
    entityIndexQueue[writeOffset] = entityIndex;
    writeOffset = (writeOffset + 1) % MAX_NUM_ENTITIES;
    numIndices++;
}

void removeEntity(Entity * e)
{
    //what about other resources?
    e->flags = 0;
    queueRemovedEntityIndex(e->entityIndex);
}

static void addInitFunc(char * name, void (*init)(Entity *e))
{
    InitFunc * initFunc = getInitFunction(getEntityType(name));

	STRCPY(initFunc->name, name);
	initFunc->init = init;
    numInitFuncs++;

    SDL_assert(numInitFuncs < MAX_NUM_INIT_FUNCS);
}

bool initEntityFactory(void)
{
    memset(initFuncs, 0, sizeof(initFuncs));
    numInitFuncs = 0;
    numIndices = 0;
    readOffset = 0;
    writeOffset = 0;

    addInitFunc("Player", initPlayer);
    addInitFunc("Barrel", initBarrel);
    addInitFunc("Projectile", initProjectile);
    addInitFunc("Tree1", initTree);

    return true;
}