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

static Drawable * tiles[MAX_NUM_THREADS];
static uint32_t   numTiles[MAX_NUM_THREADS];

static Drawable * entities[MAX_NUM_THREADS];
static uint32_t   numEntities[MAX_NUM_THREADS];

void resetDraw(void)
{
    memset(numTiles, 0, sizeof(uint32_t) * MAX_NUM_THREADS);
    memset(numEntities, 0, sizeof(uint32_t) * MAX_NUM_THREADS);
}

void drawAll(void)
{
    for(uint32_t i = 0; i < MAX_NUM_THREADS; i++)
    {
        for(uint32_t j = 0; j < numTiles[i]; j++)
        {
            Drawable * tile = &tiles[i][j];
            blitSprite(tile->sprite, (int)tile->x, (int)tile->y, false, SDL_FLIP_NONE);
        }
    }

    for(uint32_t i = 0; i < MAX_NUM_THREADS; i++)
    {
        for(uint32_t j = 0; j < numEntities[i]; j++)
        {
            Drawable * entity = &entities[i][j];
            blitSprite(entity->sprite, (int)entity->x, (int)entity->y, false, SDL_FLIP_NONE);
        }
    }
}

bool initDraw(void)
{
    for(int i = 0; i < MAX_NUM_THREADS; i++)
    {
        tiles[i] = allocateTransientMemory(MAX_TILE_COUNT_PER_THREAD * sizeof(Drawable));
        numTiles[i] = 0;
        if(tiles[i] == NULL)
        {
            return false;
        }

        entities[i] = allocateTransientMemory(MAX_ENTITY_COUNT_PER_THREAD * sizeof(Drawable));
        numEntities[i] = 0;
        if(entities[i] == NULL)
        {
            return false;
        }
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
        result = &entities[thread][numEntities[thread]];
        numEntities[thread]++;
        SDL_assert(numEntities[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
    }
    else
    {
        result = &tiles[thread][numTiles[thread]];
        numTiles[thread]++; 
        SDL_assert(numTiles[thread] <= MAX_ENTITY_COUNT_PER_THREAD);
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