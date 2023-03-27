#ifndef TEXTURE_H_
#define TEXTURE_H_

SDL_Texture * toTexture(SDL_Surface * surface, bool destroySurface);

Texture * loadTexture(char *fileName);
Texture * getTextureByIndex(uint32_t index);
uint32_t  getTextureIndex(char * fileName);
Texture * getTexture(char * fileName);
Texture * createTexture(char * fileName, int * new);

#endif