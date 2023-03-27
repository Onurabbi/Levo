#include "../common.h"

#include "../system/atlas.h"
#include "../system/input.h"
#include "../system/text.h"
#include "../system/draw.h"
#include "../system/utils.h"
#include "../system/job.h"
#include "../system/memory.h"

#include "entityFactory.h"
#include "entity.h"
#include "dungeon.h" 
#include "collision.h"

extern Dungeon dungeon;
extern App     app;

static double   accumulator;
static uint32_t ticks;
static char     fpsBuf[MAX_NAME_LENGTH];

static void initTile(MapTile * tile, char * spritePath, int tileType)
{
    tile->sprite = getSprite(spritePath);
    tile->tile = tileType;
}

static inline MapTile * getTileAtRowColRaw(int row, int col)
{
    MapTile * result = &dungeon.map[row * MAP_WIDTH + col];
    result->p.x = (float)col;
    result->p.y = (float)row;
    return result;
}

static void createDungeon(void)
{
    int oldFloor;
    char text[MAX_DESCRIPTION_LENGTH];

    dungeon.floor = dungeon.newFloor;
    dungeon.boss = NULL;

    for(int row = 0; row < MAP_HEIGHT; row++)
    {
        for(int col = 0; col < MAP_WIDTH; col++)
        {
            MapTile * tile = getTileAtRowColRaw(row, col);

            if(col == row)
            {
                initTile(tile, "gfx/tiles/2.png", TILE_WALL);
            }
            else if(row == MAP_HEIGHT/2)
            {
                initTile(tile, "gfx/tiles/12.png", TILE_GROUND);
            }
            else
            {
                initTile(tile, "gfx/tiles/0.png", TILE_GROUND);
            }
        }
    }
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

    dungeon.camera.w = MAP_RENDER_WIDTH;
    dungeon.camera.h = MAP_RENDER_HEIGHT;

    dungeon.camera.x = dungeon.player->p.x - dungeon.camera.w / 2;
    dungeon.camera.y = dungeon.player->p.y - dungeon.camera.h / 2;

    Entity * e = initEntity("Barrel");
    e->p.x = dungeon.player->p.x + 5;
    e->p.y = dungeon.player->p.y + 5;

    e = initEntity("Barrel");
    e->p.x = dungeon.player->p.x - 5;
    e->p.y = dungeon.player->p.y - 5;
    
    e = initEntity("Barrel");
    e->p.x = dungeon.player->p.x + 5;
    e->p.y = dungeon.player->p.y - 5;
    
    e = initEntity("Barrel");
    e->p.x = dungeon.player->p.x - 5;
    e->p.y = dungeon.player->p.y + 5;

    app.gameMode = GAME_MODE_DUNGEON;

    printf("Dungeon initialized!\n");
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
        Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        
        if(rectangleRectangleCollision(tileRect, screenRect) == true)
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

static void update(void)
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

void drawFPS(void)
{
    double fps = (1.0 / app.input.secElapsed);

    if(ticks == 0) sprintf(fpsBuf, "FPS: %.1f", fps);

    ticks++;
    
    if(ticks == FPS_DRAW_TICKS) ticks = 0;

    app.fontScale = 1;

    drawText(fpsBuf, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 100, 255, 255, 255, TEXT_ALIGN_CENTER, 0);
}

void updateAndRenderDungeon(void)
{
    accumulator += app.input.secElapsed;
    
    while(accumulator > 1.0/61.0)
    {
        update();

        accumulator -= 1.0/59.0;

        if(accumulator < 0.0)
        {
            accumulator = 0.0;
        }
    }

    drawAll();
    drawFPS();
}
