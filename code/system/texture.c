#include <SDL2/SDL_image.h>

#include "../common.h"

#include "memory.h"

#include "asset.h"
#include "utils.h"
#include "texture.h"

extern App app;

Texture * getTextureByIndex(uint32_t index)
{
    return (Texture *)getAssetByIndex(AT_TEXTURE, index);
}

uint32_t getTextureIndex(char * fileName)
{
    return getAssetIndex(AT_TEXTURE, fileName);
}

Texture * getTexture(char * fileName)
{
    return (Texture *)getAsset(AT_TEXTURE, fileName);
}

Texture * createTexture(char * fileName, int * new)
{
    return (Texture *)createAsset(AT_TEXTURE, fileName, new);
}

SDL_Texture * toTexture(SDL_Surface * surface, bool destroySurface)
{
    SDL_Texture * result;
    
    result = SDL_CreateTextureFromSurface(app.renderer, surface);

    if(destroySurface == true)
    {
        SDL_FreeSurface(surface);
    }

    return result;
}

Texture * loadTexture(char *fileName)//this probably needs error code
{
    Texture * result;
    SDL_Texture * newTexture;
    int new;

    result = createTexture(fileName, &new);

    if(new == 1)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ...", fileName);
        newTexture = IMG_LoadTexture(app.renderer, fileName);
        uint32_t nameLen = strlen(fileName);

        STRNCPY(result->fileName.data, fileName, nameLen);
        result->texture = newTexture;
    }
    else
    {
        //No need to load
    }

    return result;
}