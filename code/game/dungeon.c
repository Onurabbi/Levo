#include "../common.h"

#include "../system/atlas.h"
#include "../system/input.h"
#include "../system/text.h"
#include "../system/draw.h"
#include "../system/utils.h"
#include "../system/job.h"
#include "../system/memory.h"
#include "../json/cJSON.h"

#include "entityFactory.h"
#include "entity.h"
#include "dungeon.h" 
#include "collision.h"

extern Dungeon dungeon;
extern App     app;

static int walkableTiles[] = {0, 1};

static inline bool isTileWalkable(int tile)
{
    return ((tile == walkableTiles[0]) || (tile == walkableTiles[1]));
}

static void initTile(MapTile * tile, char * spritePath, int tileType)
{
    if(spritePath)
    {
        tile->sprite = getSprite(spritePath);
        tile->tile = tileType;
    }
    else
    {
        tile->sprite = NULL;
        tile->tile = TILE_NULL;
    }
}

MapTile * getTileAtRowColLayerRaw(int row, int col, int layer)
{
    MapTile * result = &dungeon.map[layer * (MAP_WIDTH * MAP_HEIGHT) + row * MAP_WIDTH + col];
    return result;
}

static MapTile * initTileAtRowColLayerRaw(int row, int col, int layer)
{
    MapTile * result = getTileAtRowColLayerRaw(row, col, layer);
    result->p.x = (float)col;
    result->p.y = (float)row;
    return result;
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
        int width = cJSON_GetObjectItem(node, "width")->valueint;
        int height = cJSON_GetObjectItem(node, "height")->valueint;

        cJSON * arr = cJSON_GetObjectItem(node, "data");

        int tileType;

        int col = 0;
        int row = 0;

        for(cJSON * child = arr->child; child != NULL; child = child->next)
        {
            MapTile * tile = initTileAtRowColLayerRaw(row, col, layer);
            int val = child->valueint; //tiled increments values
            memset(buf, 0, 64);

            char * spritePath;

            if(val > 0)
            {
                tileType = (isTileWalkable(val)) ? TILE_GROUND : TILE_WALL;
                sprintf(buf, "gfx/tiles/grasslands%d.png", val-1);
                spritePath = buf;
            }
            else
            {
                spritePath = NULL;
            }
            initTile(tile, spritePath, tileType);

            if (++col == width)
            {
                col = 0;
                row++;
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

    loadLevel("../data/test.tmj");
}

bool initDungeon(void)
{
    printf("Init dungeon!\n");

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

    dungeon.map = allocatePermanentMemory(MAX_NUM_TILES * sizeof(MapTile));
    if(dungeon.map == NULL)
    {
        printf("can't allocate map memory!\n");
        return false;
    }
    dungeon.numTiles = 0;

    createDungeon();

    dungeon.player = initEntity("Player");
    dungeon.player->p.x = (float)(MAP_WIDTH/2);
    dungeon.player->p.y = (float)(MAP_HEIGHT/2);
    
    Entity* barrel = initEntity("Barrel");
    barrel->p.x = dungeon.player->p.x + 5;
    barrel->p.y = dungeon.player->p.y + 5;
    
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

        if(isEntityAlive(entity) && rectangleRectangleCollision(entityRect, screenRect) == true)
        {
            //entity visibility will be resolved later.
            addEntityToDrawList(entity, screenPos, thread);
            addEntityToUpdateList(i, thread);
        }
    }

    finishThreadJob();
}

//just reduce the number of entities to be simulated
static void doEntityFirstPass(void)
{
    performJobs(entityFirstPassJob);
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
        
        if((rectangleRectangleCollision(tileRect, screenRect) == true) && 
           (tile->tile != TILE_NULL))
        {
            addTileToDrawList(tile, screenPos, thread);
        }
    }

    finishThreadJob();
}

static void findVisibleTiles(void)
{
    performJobs(tileVisibilityJob);
}

static void reset(void)
{
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
