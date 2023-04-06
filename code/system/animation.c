#include "../common.h"
#include "../json/cJSON.h"

#include "../system/memory.h"

#include "utils.h"
#include "asset.h"
#include "atlas.h"
#include "animation.h"

extern App app;

AnimationGroup * getAnimationGroupByIndex(uint32_t index)
{
    return (AnimationGroup *)getAssetByIndex(AT_ANIMATION_GROUP, index);
}

uint32_t getAnimationGroupIndex(char * fileName)
{
    return getAssetIndex(AT_ANIMATION_GROUP, fileName);
}

AnimationGroup *getAnimationGroup(char * fileName)
{
    return (AnimationGroup *)getAsset(AT_ANIMATION_GROUP, fileName);
}

AnimationGroup *createAnimationGroup(char * fileName, int * new)
{
    return (AnimationGroup *)createAsset(AT_ANIMATION_GROUP, fileName, new);
}

static bool loadAnimationData(char * filePath)
{
    char * text = readFile(filePath);

    if (text == NULL)
    {
        return false;
    }

    cJSON * root = cJSON_Parse(text);

    if(root == NULL)
    {
        free(text);
        return false;
    }

    for(cJSON * node = root->child; node != NULL; node = node->next)
    {
        char * fileName = cJSON_GetObjectItem(node, "filename")->valuestring;

        uint32_t nameLen = strlen(fileName);

        int new;

        AnimationGroup *animGroup = createAnimationGroup(fileName, &new);
        
        if(new == 1)
        {
            STRNCPY(animGroup->fileName, fileName, nameLen);
    
            animGroup->animationState = 0;

            int numBodyParts = cJSON_GetObjectItem(node, "numbodyparts")->valueint;

            cJSON * animations = cJSON_GetObjectItem(node, "animations");

            int animIndex = 0;

            for(cJSON * animNode = animations->child; animNode != NULL; animNode = animNode->next)
            {
                Animation * currentAnimation = &animGroup->animations[animIndex++];

                currentAnimation->numFrames = cJSON_GetObjectItem(animNode, "numframes")->valueint;
                currentAnimation->numBodyParts = numBodyParts;
                currentAnimation->frames    = allocatePermanentMemory(currentAnimation->numFrames * currentAnimation->numBodyParts * sizeof(uint32_t));

                char * animFileName = cJSON_GetObjectItem(animNode, "filename")->valuestring;

                currentAnimation->lengthSeconds = cJSON_GetObjectItem(animNode, "lengthseconds")->valuedouble;

                cJSON * frames = cJSON_GetObjectItem(animNode, "frames");

                int frameCount = 0;

                for(cJSON * frameNode = frames->child; frameNode != NULL; frameNode = frameNode->next)
                {
                    int bodyPart = frameCount % currentAnimation->numBodyParts;
                    int frameIndex = frameCount / currentAnimation->numBodyParts;

                    char * spritePath = frameNode->valuestring;
                    int index = frameIndex * numBodyParts + bodyPart;
                    currentAnimation->frames[index] = getSpriteIndex(spritePath);
                    printf("loaded animation: %s, %d\n", spritePath, index);
                    frameCount++;
                }
            }
        }
    }

    free(text);
    cJSON_Delete(root);

    return true;
}

bool initAnimations(void)
{
    if(loadAnimationData("../data/heroineUnarmedAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load heroine animation data!\n");
        return false;
    }

    if(loadAnimationData("../data/longswordAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load longsword animation data!\n");
        return false;
    }

    return true;
}