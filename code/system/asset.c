#include "../common.h"

#include "memory.h"
#include "utils.h"
#include "asset.h"
#include "animation.h"
#include "atlas.h"
#include "texture.h"

static AnimationGroup * animationGroups;
static uint32_t         numAnimationGroups;

static Sprite *  sprites;
static uint32_t  numSprites;

static Texture * textures;
static uint32_t  numTextures;

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
        case AT_ANIMATION_GROUP:
            result = (uint8_t *)animationGroups;
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
            SDL_assert(numTextures <= MAX_NUM_TEXTURES);
            break;
        case AT_SPRITE:
            numSprites++;
            SDL_assert(numSprites <= MAX_NUM_SPRITES);
            break;
        case AT_ANIMATION_GROUP:
            numAnimationGroups++;
            SDL_assert(numAnimationGroups <= MAX_NUM_ANIMATION_GROUPS);
            break;
        default:
            break;
    }
}

static size_t getAssetSize(AssetType type)
{
    size_t result = 0;

    switch(type)
    {
        case AT_TEXTURE:
            result = sizeof(Texture);
            break;
        case AT_SPRITE:
            result = sizeof(Sprite);
            break;
        case AT_ANIMATION_GROUP:
            result = sizeof(AnimationGroup);
            break;
        default:
            break;
    }

    return result;
}

static uint32_t getMaxAssetCount(AssetType type)
{
    uint32_t result = 0;

    switch(type)
    {
        case AT_TEXTURE:
            result = MAX_NUM_TEXTURES;
            break;
        case AT_SPRITE:
            result = MAX_NUM_SPRITES;
            break;
        case AT_ANIMATION_GROUP:
            result = MAX_NUM_ANIMATION_GROUPS;
            break;
        default:
            break;
    }

    return result;   
}

uint8_t * getAssetByIndex(AssetType assetType, uint32_t index)
{
    uint8_t * assetStore = getAssetStore(assetType);
    size_t size = getAssetSize(assetType);
    return (assetStore + index * size);
}

uint8_t * getAsset(AssetType assetType, char * fileName)
{
    uint8_t * result;

    uint8_t * assetStore = getAssetStore(assetType);

    size_t size = getAssetSize(assetType);

    uint32_t maxCount = getMaxAssetCount(assetType);

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

uint8_t * createAsset(AssetType assetType, char * fileName, int *new)
{
    uint8_t * result;

    uint8_t * assetStore = getAssetStore(assetType);

    size_t size = getAssetSize(assetType);

    uint32_t maxCount = getMaxAssetCount(assetType);

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

uint32_t getAssetIndex(AssetType assetType, char * fileName)
{
    uint8_t * assetStore = getAssetStore(assetType);

    size_t size = getAssetSize(assetType);

    uint32_t maxCount = getMaxAssetCount(assetType);
    
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
    animationGroups = allocatePermanentMemory(MAX_NUM_ANIMATION_GROUPS * sizeof(AnimationGroup));

    if(animationGroups == NULL)
    {
        return false;
    }

    numAnimationGroups = 0;

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