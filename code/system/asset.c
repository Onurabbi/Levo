#include "../common.h"

#include "memory.h"
#include "utils.h"
#include "asset.h"
#include "animation.h"
#include "atlas.h"
#include "texture.h"

static Animation * animations;
static uint32_t    numAnimations;
static Sprite    * sprites;
static uint32_t    numSprites;
static Texture   * textures;
static uint32_t    numTextures;

static inline uint8_t * getAssetStore(AssetType type)
{
    uint8_t * result;

    switch(type)
    {
        case AT_TEXTURE:
            result = (uint8_t *)textures;
            break;
        case AT_SPRITE:
            result = (uint8_t *)sprites;
            break;
        case AT_ANIMATION:
            result = (uint8_t *)animations;
            break;
        default:
            result = NULL;
            break;
    }
    return result;
}

static void incrementAssetCount(AssetType type)
{
    switch(type)
    {
        case AT_TEXTURE:
            numTextures++;
            SDL_assert(numSprites <= MAX_NUM_TEXTURES);
            break;
        case AT_SPRITE:
            numSprites++;
            SDL_assert(numSprites <= MAX_NUM_SPRITES);
            break;
        case AT_ANIMATION:
            numAnimations++;
            SDL_assert(numAnimations <= MAX_NUM_ANIMATIONS);
            break;
        default:
            break;
    }
}

uint8_t * getAssetByIndex(AssetType assetType, uint32_t index, size_t size)
{
    uint8_t * assetStore = getAssetStore(assetType);
    return (assetStore + index * size);
}

uint8_t * getAsset(AssetType assetType, char * fileName, size_t size, uint32_t maxCount)
{
    uint8_t * result;

    uint8_t * assetStore = getAssetStore(assetType);

    uint32_t i = hashcode(fileName, strlen(fileName)) % maxCount;

    for(int index = i; index != i - 1; index++)
    {
        index %= maxCount;
        const char *assetName = (const char *)assetStore + (index * size);
        if(strcmp(fileName, assetName) == 0)
        {
            result = (uint8_t *)assetName;
            return result;
        }
    }
    return NULL;
}

uint8_t * createAsset(AssetType assetType, char * fileName, size_t size, uint32_t maxCount, int *new)
{
    uint8_t * result;

    uint8_t * assetStore = getAssetStore(assetType);

    uint32_t i = hashcode(fileName, strlen(fileName)) % maxCount;

    *new = 0;

    for(int index = i; index != i - 1; index++)
    {
        index %= maxCount;
        result = assetStore + (index * size);
        const char * assetName = (const char *)result;
        if(strcmp(fileName, assetName) == 0)
        {
            return result;
        }
        else if(*result == 0) //uninitialized
        {
            incrementAssetCount(assetType);
            *new = 1;
            return result;
        }
    }
    return NULL;
}

uint32_t getAssetIndex(AssetType assetType, char * fileName, size_t size, uint32_t maxCount)
{
    uint8_t * assetStore = getAssetStore(assetType);

    uint32_t i = hashcode(fileName, strlen(fileName)) % maxCount;

    for(int index = i; index != i - 1; index++)
    {
        index %= maxCount;
        const char *assetName = (const char *)assetStore + (index * size);
        if(strcmp(fileName, assetName) == 0)
        {
            return index;
        }
    }
    return maxCount;
}

bool initAssets(void)
{
    animations = allocatePermanentMemory(MAX_NUM_ANIMATIONS * sizeof(Animation));

    if(animations == NULL)
    {
        return false;
    }

    numAnimations = 0;

    sprites = allocatePermanentMemory(MAX_NUM_SPRITES * sizeof(Sprite));

    if(sprites == NULL)
    {
        return false;
    }

    numSprites = 0;
    
    textures = allocatePermanentMemory(MAX_NUM_TEXTURES * sizeof(Texture));

    if (textures == NULL)
    {
        return false;
    }    
    
    numTextures = 0;

    return true;
}