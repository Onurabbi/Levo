#include "../common.h"
#include "../json/cJSON.h"

#include "utils.h"
#include "asset.h"
#include "atlas.h"
#include "animation.h"

extern App app;

Animation * getAnimationByIndex(uint32_t index)
{
    return (Animation *)getAssetByIndex(AT_ANIMATION, index, sizeof(Animation));
}

uint32_t getAnimationIndex(char * fileName)
{
    return getAssetIndex(AT_ANIMATION, fileName, sizeof(Animation), MAX_NUM_ANIMATIONS);
}

Animation * getAnimation(char * fileName)
{
    return (Animation *)getAsset(AT_ANIMATION, fileName, sizeof(Animation), MAX_NUM_ANIMATIONS);
}

Animation * createAnimation(char * fileName, int * new)
{
    return (Animation *)createAsset(AT_ANIMATION, fileName, sizeof(Animation), (uint32_t)MAX_NUM_ANIMATIONS, new);
}

static bool loadAnimationData(char * filePath)
{
    char * text = readFile(filePath);

    if (text == NULL)
    {
        return false;
    }

    cJSON * root = cJSON_Parse(text);

    for(cJSON * node = root->child; node != NULL; node = node->next)
    {
        char * fileName = cJSON_GetObjectItem(node, "filename")->valuestring;
        
        uint32_t nameLen = strlen(fileName);

        int new;

        Animation * anim = createAnimation(fileName, &new);

        if(new == 1)
        {
            int numFrames = cJSON_GetObjectItem(node, "numFrames")->valueint;
            
            SDL_assert(numFrames > 0 && numFrames < MAX_ANIMATION_SPRITES);

            anim->secPerFrame = cJSON_GetObjectItem(node, "secperframe")->valuedouble;
            
            anim->numSpriteIndices = numFrames;
            STRNCPY(anim->fileName, fileName, nameLen);
            
            char frameId[MAX_NAME_LENGTH];

            for (int i = 0; i < numFrames; i++)
            {
                sprintf(frameId, "frame%d", i);
                char * framePath = cJSON_GetObjectItem(node, frameId)->valuestring;               
                anim->spriteIndices[i] = getSpriteIndex(framePath);
            }
            anim->currentSpriteIndex = 0;
            anim->AnimTimer = 0.0;
        }
    }
    return true;
}

bool initAnimations(void)
{
    if(loadAnimationData("../data/nordicladyanim.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load animation data!\n");
        return false;
    }
    return true;
}