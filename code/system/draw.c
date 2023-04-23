#include "../common.h"

#include "../game/astar.h"

#include "vector.h"
#include "job.h"
#include "memory.h"
#include "utils.h"
#include "atlas.h"
#include "draw.h"

extern App app;
//TODO: camera should probably be somewhere else
extern Dungeon dungeon;

//not using this for now
static Sprite *  pointerSprite;

static Drawable *tiles;
static uint32_t numTiles[MAX_NUM_THREADS];

static Drawable *entities;
static uint32_t numEntities[MAX_NUM_THREADS];

static SDL_FPoint *debugPoints;
static uint32_t   numDebugPoints[MAX_NUM_THREADS];

static uint32_t totalNumTiles;
static uint32_t totalNumEntities;

static inline SDL_FPoint convertToIso(SDL_FPoint point)
{
    SDL_FPoint result;
    toIso(point.x, point.y, &result.x, &result.y);
    return result;
}

void debugDrawRect(Vec2f start, float w, float h, int r, int g, int b, int thread)
{
    if (numDebugPoints[thread] == MAX_NUM_DEBUG_POINTS/MAX_NUM_THREADS)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Debug draw queue is full!\n");
        return;
    }

    if (r > 255) r = 255;
    else if (r < 0) r = 0;
    
    if (g > 255) g = 255;
    else if (g < 0) g = 0;

    if (b > 255) b = 255;
    else if (b < 0) b = 0;

    float x,y;
    uint32_t pointsPerThread = MAX_NUM_DEBUG_POINTS / MAX_NUM_THREADS;

    SDL_FPoint *base = debugPoints + thread * pointsPerThread;
    SDL_assert(thread * pointsPerThread < MAX_NUM_DEBUG_POINTS);

    SDL_FPoint topLeft;
    topLeft.x = start.x - w/2;
    topLeft.y = start.y - h/2;
    topLeft = convertToIso(topLeft);
    base[numDebugPoints[thread]++] = topLeft;

    SDL_FPoint topRight;
    topRight.x = start.x + w/2;
    topRight.y = start.y - h/2;
    topRight = convertToIso(topRight);
    base[numDebugPoints[thread]++] = topRight;

    SDL_FPoint bottomRight;
    bottomRight.x = start.x + w/2;
    bottomRight.y = start.y + h/2;
    bottomRight = convertToIso(bottomRight);
    base[numDebugPoints[thread]++] = bottomRight;

    SDL_FPoint bottomLeft;
    bottomLeft.x = start.x - w/2;
    bottomLeft.y = start.y + h/2;
    bottomLeft = convertToIso(bottomLeft);
    base[numDebugPoints[thread]++] = bottomLeft;
}

static int compareDrawables(const void * a, const void * b)
{
    SDL_assert(a && b);

    Drawable * d1 = (Drawable *)a;
    Drawable * d2 = (Drawable *)b;

    if (d1->y < d2->y)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

void denselyPackAndSortDrawableEntities(void)
{
    Drawable *dest =  entities + numEntities[0];
    totalNumEntities = numEntities[0];
    for(int i = 0; i < MAX_NUM_THREADS - 1; i++)
    {
        Drawable *src = entities + (i+1) * MAX_ENTITY_COUNT_PER_THREAD;
        uint32_t bytes = numEntities[i+1] * sizeof(Drawable);

        memcpy(dest, src, bytes);

        dest += numEntities[i+1];
        totalNumEntities += numEntities[i+1];
    }
    qsort(entities, totalNumEntities, sizeof(Drawable), compareDrawables);
}

void denselyPackDrawableTiles(void)
{
    Drawable *dest = tiles + numTiles[0];
    totalNumTiles = numTiles[0];

    for(int i = 0; i < MAX_NUM_THREADS - 1; i++)
    {
        Drawable *src = tiles + (i+1) * MAX_TILE_COUNT_PER_THREAD;
        uint32_t bytes = numTiles[i+1] * sizeof(Drawable);

        memcpy(dest, src, bytes);

        dest += numTiles[i+1];
        totalNumTiles += numTiles[i+1];
    }
}

void resetDraw(void)
{
    for(int thread = 0; thread < MAX_NUM_THREADS; thread++)
    {
        numEntities[thread] = 0;
        numTiles[thread] = 0;
        numDebugPoints[thread] = 0;
    }
    totalNumEntities = 0;
    totalNumTiles = 0;
}

static Drawable * getDrawableEntity(uint32_t i, uint32_t thread)
{
    Drawable * base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    return &base[i];
}

static void setDrawableEntity(Drawable drawable, uint32_t thread)
{
    Drawable * base = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
    base[numEntities[thread]] = drawable;
}

static Drawable * getDrawableTile(uint32_t i, uint32_t thread)
{
    Drawable * base = tiles + thread * MAX_TILE_COUNT_PER_THREAD;
    return &base[i]; 
}

static void setDrawableTile(Drawable drawable, uint32_t thread)
{
    Drawable * base = tiles + thread * MAX_TILE_COUNT_PER_THREAD;
    base[numTiles[thread]] = drawable;
}

static void drawDebugLines(void)
{
    uint32_t pointsPerThread = MAX_NUM_DEBUG_POINTS / MAX_NUM_THREADS;
    uint32_t pointsPerRect = 4;

    SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);

    for (int thread = 0; thread < MAX_NUM_THREADS; thread++)
    {
        int numRects = numDebugPoints[thread] / pointsPerRect;
        for(int rect = 0; rect < numRects; rect++)
        {
            for (int point = 0; point < pointsPerRect; point++)
            {
                SDL_FPoint *currentRectangle = debugPoints + thread * pointsPerThread + rect * pointsPerRect;
                int index = point;
                int nextIndex = (point+1) % pointsPerRect;
                SDL_FPoint start = currentRectangle[index];
                SDL_FPoint end = currentRectangle[nextIndex];
                SDL_RenderDrawLineF(app.renderer, (int)start.x, (int)start.y, (int)end.x, (int)end.y);
            }
        }
    }
}

void drawAll(void)
{
    for (int i = 0; i < totalNumTiles; i++)
    {
        blitSprite(tiles[i].sprite, (int)tiles[i].x, (int)tiles[i].y, true, SDL_FLIP_NONE);
    }
    for (int i = 0; i < totalNumEntities; i++)
    {
        blitSprite(entities[i].sprite, (int)entities[i].x,(int)entities[i].y, true, SDL_FLIP_NONE);
    }

    drawDebugLines();
}

bool initDraw(void)
{
    tiles = allocatePermanentMemory(MAX_NUM_TILES * sizeof(Drawable));
    if(tiles == NULL)
    {
        return false;
    }
    
    entities = allocatePermanentMemory(MAX_NUM_ENTITIES * sizeof(Drawable));
    if(entities == NULL)
    {
        return false;
    }

    debugPoints = allocatePermanentMemory(MAX_NUM_DEBUG_POINTS * sizeof(SDL_FPoint));
    if (debugPoints == NULL)
    {
        return false;
    }
    return true;
}

void prepareScene(void)
{
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
    if (app.showPointer && app.gameMode == GAME_MODE_DUNGEON)
    {
        drawOutlineRect(app.mouseScreenPosition.x - 4, app.mouseScreenPosition.y - 4, 8, 8, 255, 255, 255, 255);
    }
    SDL_RenderPresent(app.renderer);
}

void blit(SDL_Texture * texture, int x, int y, bool center)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    if (center)
    {
        dest.x -= dest.w / 2;
        dest.y -= dest.h / 2;
    }
    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitSprite(Sprite * sprite, int x, int y, bool center, SDL_RendererFlip flip)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = sprite->rect.w * ZOOM;
    dest.h = sprite->rect.h * ZOOM;
    if (center)
    {
        dest.x -= dest.w / 2;
        dest.y -= dest.h / 2;
    }
    SDL_RenderCopyEx(app.renderer, sprite->texture, &sprite->rect, &dest, 0.0, NULL, flip);
}

void drawRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
    SDL_RenderFillRect(app.renderer, &rect);
}

void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
    SDL_RenderDrawRect(app.renderer, &rect);
}

static Drawable * getNewDrawable(int layer, int thread)
{
    Drawable * result = NULL;

    if(layer == DL_ENTITY)
    {
        result = getDrawableEntity(numEntities[thread], thread);
        numEntities[thread]++;
        SDL_assert(numEntities[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
    }
    else
    {
        result = getDrawableTile(numTiles[thread], thread);
        numTiles[thread]++; 
        SDL_assert(numTiles[thread] <= MAX_TILE_COUNT_PER_THREAD);
    }
    
    return result;
}

void addEntityToDrawList(Entity * e, Vec2f p, int thread)
{
    for(int i = 0; i < e->entitySprites.drawableCount; i++)
    {   
        Drawable * drawable = getNewDrawable(DL_ENTITY, thread);
        if(drawable)
        {
            drawable->sprite = e->entitySprites.sprites[i];
            drawable->x = p.x;
            drawable->y = p.y + e->entitySprites.yOffsets[i];
        }
        else
        {
            printf("Can't get drawable\n");
        }
    }
}

void addTileToDrawList(MapTile * tile, Vec2f p, int thread)
{
    Drawable * drawable = getNewDrawable(DL_TILE, thread);
    if(drawable)
    {
        drawable->sprite = tile->sprite;
        drawable->x = p.x;
        drawable->y = p.y;
    }
    else
    {
        printf("Can't get drawable\n");
    }
}