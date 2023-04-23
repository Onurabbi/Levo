#include "../common.h"

#include "../system/atlas.h"
#include "tile.h"

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