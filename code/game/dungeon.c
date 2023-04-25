#include "../common.h"

#include "../system/atlas.h"
#include "../system/input.h"
#include "../system/text.h"
#include "../system/draw.h"
#include "../system/utils.h"
#include "../system/job.h"
#include "../system/memory.h"
#include "../json/cJSON.h"

#include "tile.h"
#include "astar.h"
#include "entityFactory.h"
#include "entity.h"
#include "dungeon.h" 
#include "collision.h"

extern Dungeon dungeon;
extern App     app;

static int walkableTiles[] = {0, 1};

static inline bool isTileWalkable(int tile)
{
    for (int i = 0; i < ArrayCount(walkableTiles); i++)
    {
        if (tile == walkableTiles[i])
        {
            return true;
        }
    }
    return false;
}

static void loadLevel(char *path)
{
    char * text = readFile(path);
    if(text == NULL)
    {
        return;
    }

    cJSON * root = cJSON_Parse(text);

    if(root == NULL)
    {
        free(text);
        return;
    }

    cJSON * layers = cJSON_GetObjectItem(root, "layers");

    if(layers == NULL)
    {
        free(text);
        cJSON_Delete(root);
        return;
    }

    char buf[64];
    int layer = 0;

    for(cJSON * node = layers->child; node != NULL; node = node->next)
    {
        SDL_assert(layer < MAX_NUM_TILE_SPRITES);

        int width = cJSON_GetObjectItem(node, "width")->valueint;
        int height = cJSON_GetObjectItem(node, "height")->valueint;

        cJSON * arr = cJSON_GetObjectItem(node, "data");

        int tileType;

        int col = 0;
        int row = 0;

        for(cJSON * child = arr->child; child != NULL; child = child->next)
        {
            MapTile *tile = NULL;
            if (layer == 0)
            {
                tile = initTileAtRowCol(dungeon.map, row, col);
            }
            else
            {
                tile = getTileAtRowCol(dungeon.map, row, col);
            }

            if (tile != NULL)
            {
                int val = child->valueint; //tiled increments value
                if(val > 0)//a tile exists
                {
                    memset(buf, 0, 64);
                    sprintf(buf, "gfx/tiles/grasslands%d.png", val - 1);
                    bool walkable = isTileWalkable(val);
                    if (walkable == false)
                    {
                        printf("tile %s is not walkable\n", buf);
                        BIT_SET(tile->flags, TILE_CAN_COLLIDE_BIT);
                    }
                    addSpriteToTile(tile, buf);
                }
                if (++col == width)
                {
                    col = 0;
                    row++;
                }
            }
            else
            {
                //TODO: Logging
            }
        }

        layer++;
    }
    free(text);
    cJSON_Delete(root);
}

static void createDungeon(void)
{
    int oldFloor;
    char text[MAX_DESCRIPTION_LENGTH];

    dungeon.floor = dungeon.newFloor;
    dungeon.boss = NULL;

    loadLevel("../data/test2.tmj");
}

bool initDungeon(void)
{
    memset(&dungeon, 0, sizeof(Dungeon));

    dungeon.entities = allocatePermanentMemory(MAX_NUM_ENTITIES * sizeof(Entity));
    if(dungeon.entities == NULL)
    {
        printf("can't allocate dungeon memory\n");
        return false;
    }
    dungeon.numEntities = 0;

    dungeon.actors = allocatePermanentMemory(MAX_NUM_ACTORS * sizeof(Actor));
    if(dungeon.actors == NULL)
    {
        printf("can't allocate actor memory!\n");
        return false;
    }
    dungeon.numActors = 0;

    dungeon.weapons = allocatePermanentMemory(MAX_NUM_WEAPONS * sizeof(Weapon));
    if(dungeon.weapons == NULL)
    {
        printf("can't allocate weapon memory!\n");
        return false;
    }
    dungeon.numWeapons = 0;

    dungeon.map = allocatePermanentMemory(MAX_NUM_TILES * sizeof(MapTile));
    if(dungeon.map == NULL)
    {
        printf("can't allocate map memory!\n");
        return false;
    }
    dungeon.numTiles = 0;

    dungeon.animationControllers = allocatePermanentMemory(MAX_NUM_ANIMATION_CONTROLLERS * sizeof(AnimationController));
    if(dungeon.animationControllers == NULL)
    {
        printf("can't allocate animation control memory");
        return false;
    }
    dungeon.numAnimationControllers = 0;
    
    createDungeon();

    dungeon.player = initEntity("Player");
    dungeon.player->p.x = (float)(MAP_WIDTH/2);
    dungeon.player->p.y = (float)(MAP_HEIGHT/2);

    Entity *sword = initEntity("Longsword");
    sword->p.x = dungeon.player->p.x - 5;
    sword->p.y = dungeon.player->p.y - 5;

    Entity *enemy = initEntity("Enemy");
    enemy->p.x = dungeon.player->p.x - rand() % 10;
    enemy->p.y = dungeon.player->p.y + rand() % 10;

    dungeon.camera.w = MAP_RENDER_WIDTH;
    dungeon.camera.h = MAP_RENDER_HEIGHT;

    dungeon.camera.x = dungeon.player->p.x - dungeon.camera.w / 2;
    dungeon.camera.y = dungeon.player->p.y - dungeon.camera.h / 2;

    app.gameMode = GAME_MODE_DUNGEON;
    return true;
}

static bool cursorOverScreenRect(Rect entityRect)
{
    return pointRectangleCollisioni(entityRect, app.mouseScreenPosition);
}

static void entityFirstPassJob(void * context)
{
    int thread = *((int *)context);
    int startIndex = thread * MAX_ENTITY_COUNT_PER_THREAD;
    int endIndex = (thread + 1) * MAX_ENTITY_COUNT_PER_THREAD;

    for(int i = startIndex; i < endIndex; i++)
    {
        Entity * entity = &dungeon.entities[i];
        Vec2f screenPos;
        toIso(entity->p.x, entity->p.y, &screenPos.x, &screenPos.y);
        Rect entityRect = {screenPos.x, screenPos.y, TILE_WIDTH, TILE_HEIGHT};
        Rect screenRect = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT};

        if(isEntityAlive(entity) && checkRectangleRectangleCollision(entityRect, screenRect) == true)
        {
            //entity visibility will be resolved later.
            if(isEntityVisible(entity) == true)
            {
                addEntityToDrawList(entity, screenPos, thread);
            }
            addEntityToUpdateList(i, thread);
        }
    }
    finishThreadJob();
}

//just reduce the number of entities to be simulated
static void doEntityFirstPass(void)
{
    performJobs(entityFirstPassJob);
    denselyPackEntities();
    denselyPackAndSortDrawableEntities();
}

static void tileVisibilityJob(void * context)
{
    int thread = *((int *)context);
    int startIndex = thread * MAX_TILE_COUNT_PER_THREAD;
    int endIndex = (thread + 1) * MAX_TILE_COUNT_PER_THREAD;

    for(int i = startIndex; i < endIndex; i++)
    {
        MapTile * tile = &dungeon.map[i];

        Vec2f screenPos;
        toIso(tile->p.x, tile->p.y, &screenPos.x, &screenPos.y);
        Rect tileRect = {screenPos.x, screenPos.y, TILE_WIDTH, TILE_HEIGHT};
        Rect screenRect = {0, 0, SCREEN_WIDTH + TILE_WIDTH,  SCREEN_HEIGHT + TILE_HEIGHT};
        
        if(checkRectangleRectangleCollision(tileRect, screenRect) == true)
        {
            if (BIT_CHECK(tile->flags, TILE_CAN_COLLIDE_BIT) != 0)
            {
                Vec2f rectPos;
                rectPos.x = tile->p.x;
                rectPos.y = tile->p.y;

                debugDrawRect(rectPos, 1.0f, 1.0f, 255, 0, 0, thread);
            }
            addTileToDrawList(tile, screenPos, thread);
        }
    }
    finishThreadJob();
}

static void findVisibleTiles(void)
{
    performJobs(tileVisibilityJob);
    denselyPackAndSortDrawableTiles();
}

static void reset(void)
{
    resetTransientMemory();
    resetDraw();
    resetEntityUpdates();
}

void renderDungeon(void)
{
    drawAll();
}

void updateDungeon(void)
{
    Input * input = &app.input;

    if (input->keyboardEvents[SDL_SCANCODE_ESCAPE] == 1)
    {
        clearInput(&app.input);
        app.gameMode = GAME_MODE_MENU;
        return;
    }

    app.entityOverCursorIndex = 0;
    reset();
    findVisibleTiles();
    doEntityFirstPass();
    updateEntities();
}
