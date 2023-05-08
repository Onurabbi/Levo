#include "../common.h"

#include "../system/atlas.h"

#include "collision.h"
#include "tile.h"
#include "dungeon.h"

extern Dungeon dungeon;
extern App app;

bool checkTileCollisions(Entity *e, Vec2f *deltaP)
{
    MapTile *tiles[9];
    tiles[0]  = getTileAtRowCol(dungeon.map, (int)(e->p.y), (int)(e->p.x));
    tiles[1]  = getTileAtRowCol(dungeon.map, (int)(e->p.y - 1), (int)(e->p.x - 1));
    tiles[2]  = getTileAtRowCol(dungeon.map, (int)(e->p.y - 1), (int)(e->p.x));
    tiles[3]  = getTileAtRowCol(dungeon.map, (int)(e->p.y - 1), (int)(e->p.x + 1));
    tiles[4]  = getTileAtRowCol(dungeon.map, (int)(e->p.y), (int)(e->p.x - 1));
    tiles[5]  = getTileAtRowCol(dungeon.map, (int)(e->p.y), (int)(e->p.x + 1));
    tiles[6]  = getTileAtRowCol(dungeon.map, (int)(e->p.y + 1), (int)(e->p.x - 1));
    tiles[7]  = getTileAtRowCol(dungeon.map, (int)(e->p.y + 1), (int)(e->p.x));
    tiles[8]  = getTileAtRowCol(dungeon.map, (int)(e->p.y + 1), (int)(e->p.x + 1));

    Rect rect = {e->p.x, e->p.y, e->width, e->height};

    for(int i = 0; i < 9; i++)
    {
        if (tiles[i] != NULL)
        {
            Rect tileRect = {tiles[i]->p.x, tiles[i]->p.y, 1.0f, 1.0f};
            if((BIT_CHECK(tiles[i]->flags, TILE_CAN_COLLIDE_BIT)))
            {
                (void)resolveDynamicRectangleVsRectangle(rect, deltaP, tileRect);
            }
        }
    }
    
    return false;
}

void addSpriteToTile(MapTile * tile, char * spritePath)
{
    if(spritePath)
    {
        tile->sprites[tile->spriteCount++] = getSprite(spritePath);
    }
    else
    {
        //TODO: logging
    }
}

MapTile * getTileAtRowCol(MapTile *tiles, int row, int col)
{
    MapTile *result = NULL;
    if (row >= 0 && row < MAP_HEIGHT && col >= 0 && col < MAP_WIDTH)
    {
        result = &tiles[row * MAP_WIDTH + col];
    }
    return result;
}

MapTile * initTileAtRowCol(MapTile *tiles, int row, int col)
{
    MapTile * result = getTileAtRowCol(tiles, row, col);
    result->p.x = (float)col;
    result->p.y = (float)row;
    result->flags = 0;
    result->spriteCount = 0;
    return result;
}