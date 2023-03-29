#include "../common.h"

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

static Drawable * tiles;
static uint32_t   numTiles[MAX_NUM_THREADS];

static Drawable * entities;
static uint32_t   numEntities[MAX_NUM_THREADS];

static int compareDrawables(const void * a, const void * b)
{
    SDL_assert(a && b);

    Drawable * d1 = (Drawable *)a;
    Drawable * d2 = (Drawable *)b;

    return (d2->y - d1->y);
}

void resetDraw(void)
{
    memset(tiles, 0, sizeof(Drawable) * MAX_NUM_TILES);
    memset(entities, 0, sizeof(Drawable) * MAX_NUM_ENTITIES);
    memset(numTiles, 0, sizeof(uint32_t) * MAX_NUM_THREADS);
    memset(numEntities, 0, sizeof(uint32_t) * MAX_NUM_THREADS);
}

void drawAll(void)
{
    for(uint32_t i = 0; i < MAX_NUM_THREADS; i++)
    {
        Drawable * base = tiles + i * MAX_TILE_COUNT_PER_THREAD;
        qsort(base, numTiles[i], sizeof(Drawable), compareDrawables);
    }

    for(uint32_t i = 0; i < MAX_NUM_THREADS; i++)
    {
        Drawable * base = tiles + i * MAX_TILE_COUNT_PER_THREAD;

        for(uint32_t  j = 0; j < numTiles[i]; j++)
        {
            Drawable * tile = &base[j];
            blitSprite(tile->sprite, (int)tile->x, (int)tile->y, true, SDL_FLIP_NONE);
        }
    }
    
#if 0
    for(uint32_t i = 0; i < MAX_NUM_THREADS; i++)
    {
        for(uint32_t j = 0; j < numTiles[i]; j++)
        {
            Drawable * tile = &tiles[i][j];
            blitSprite(tile->sprite, (int)tile->x, (int)tile->y, false, SDL_FLIP_NONE);
        }
    }
#endif

    for(uint32_t i = 0; i < MAX_NUM_THREADS; i++)
    {
        Drawable * base = entities + i * MAX_ENTITY_COUNT_PER_THREAD;
        for(uint32_t j = 0; j < numEntities[i]; j++)
        {
            Drawable * entity = &base[j];
            blitSprite(entity->sprite, (int)entity->x, (int)entity->y, true, SDL_FLIP_NONE);
        }
    }
}

bool initDraw(void)
{
    tiles = allocateTransientMemory(MAX_NUM_TILES * sizeof(Drawable));
    if(tiles == NULL)
    {
        return false;
    }
    
    entities = allocateTransientMemory(MAX_NUM_ENTITIES * sizeof(Drawable));
    if(entities == NULL)
    {
        return false;
    }

    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {   
        numEntities[i] = 0;
        numTiles[i] = 0;
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

/*
    x and y should be top left
*/
void blitSprite(Sprite * sprite, int x, int y, bool center, SDL_RendererFlip flip)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = sprite->rect.w;
    dest.h = sprite->rect.h;

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
        Drawable * drawables = entities + thread * MAX_ENTITY_COUNT_PER_THREAD;
        result = &drawables[numEntities[thread]];
        numEntities[thread]++;
        SDL_assert(numEntities[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
    }
    else
    {
        Drawable * drawables = tiles + thread * MAX_TILE_COUNT_PER_THREAD;
        result = &drawables[numTiles[thread]];
        numTiles[thread]++; 
        SDL_assert(numTiles[thread] <= MAX_TILE_COUNT_PER_THREAD);
    }
    
    return result;
}

void addEntityToDrawList(Entity * e, Vec2f p, int thread)
{
    Drawable * drawable = getNewDrawable(DL_ENTITY, thread);
    if(drawable)
    {
        drawable->sprite = e->sprite;
        drawable->x = p.x;
        drawable->y = p.y;
    }
    else
    {
        printf("Can't get drwaable\n");
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