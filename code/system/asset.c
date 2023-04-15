#include "../common.h"

#include "memory.h"
#include "utils.h"
#include "asset.h"
#include "animation.h"
#include "atlas.h"
#include "texture.h"

static AnimationGroup *animationGroups;
static uint32_t       numAnimationGroups;

static Animation  *animations;
static uint32_t   numAnimations;

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
        case AT_ANIMATION:
            result = (uint8_t *)animations;
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
        case AT_ANIMATION:
            numAnimations++;
            SDL_assert(numAnimations <= MAX_NUM_ANIMATIONS);
            break;
        case AT_ANIMATION_GROUP:
            numAnimationGroups++;
            SDL_assert(numAnimationGroups <= MAX_NUM_ANIMATION_GROUPS);
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
        case AT_ANIMATION:
            result = sizeof(Animation);
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
        case AT_ANIMATION:
            result = MAX_NUM_ANIMATIONS;
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
    int index = i;

    while(index != i - 1)
    {
        result = assetStore + size * index;
        String *fileNameStr = (String *)result;
        if(strcmp(fileNameStr->data, fileName) == 0)
        {
            return result;
        }
        index = (index + 1) % maxCount;
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
    int index = i;
    static int spriteCollisions;
    while(index != i-1)
    {
        result = assetStore + size * index;
        String *fileNameStr = (String *)result;

        if(fileNameStr->count == 0) //uninitialized
        {
            incrementAssetCount(assetType);
            *new = 1;

            fileNameStr->data = allocatePermanentMemory(MAX_FILENAME_LENGTH);
            fileNameStr->count = MAX_FILENAME_LENGTH;

            return result;
        }
        else
        {
            if(strcmp(fileNameStr->data, fileName) == 0)
            {
                return result;
            }
        }

        printf("sprite collisions: %d\n", spriteCollisions);
        spriteCollisions++;
        index = (index + 1) % maxCount;
    }
    return NULL;
}

uint32_t getAssetIndex(AssetType assetType, char * fileName)
{
    uint8_t * assetStore = getAssetStore(assetType);
    size_t size = getAssetSize(assetType);
    uint32_t maxCount = getMaxAssetCount(assetType);
    uint32_t i = hashcode(fileName, strlen(fileName)) % maxCount;
    int index = i;
    while(index != i - 1)
    {
        uint8_t *result = assetStore + size * index;
        String *fileNameStr = (String *)result;
        if(strcmp(fileNameStr->data, fileName) == 0)
        {
            return index;
        }
        index = (index + 1) % maxCount;
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