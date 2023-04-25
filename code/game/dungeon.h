#ifndef DUNGEON_H_
#define DUNGEON_H_

bool      initDungeon(void);
void      updateDungeon(void);
void      renderDungeon(void);
MapTile * getTileAtRowColLayerRaw(int row, int col, int layer);
#endif