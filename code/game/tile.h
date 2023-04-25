#ifndef TILE_H_
#define TILE_H_

#define TILE_CAN_COLLIDE_BIT 0
#define TILE_IS_OCCUPIED_BIT 1

MapTile * getTileAtRowCol(MapTile *tiles, int row, int col);
MapTile * initTileAtRowCol(MapTile *tiles, int row, int col);
void      addSpriteToTile(MapTile * tile, char * spritePath);
bool      checkTileCollisions(Entity *e, Vec2f *newPos);
#endif