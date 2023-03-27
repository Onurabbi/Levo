#ifndef ATLAS_H_
#define ATLAS_H_

bool     initSprites(void);
Sprite * getSpriteByIndex(uint32_t index);
uint32_t getSpriteIndex(char * fileName);
Sprite * getSprite(char * fileName);
Sprite * createSprite(char * fileName, int * new);

#endif