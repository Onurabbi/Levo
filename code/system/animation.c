#include "../common.h"
#include "../json/cJSON.h"

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

AnimationGroup * getAnimationGroup(char * fileName)
{
    return (AnimationGroup *)getAsset(AT_ANIMATION_GROUP, fileName);
}

AnimationGroup * createAnimationGroup(char * fileName, int * new)
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
            
            animGroup->numAnimations = cJSON_GetObjectItem(node, "numanims")->valueint;
            
            animGroup->animationState = 0;

            cJSON * animations = cJSON_GetObjectItem(node, "animations");

            int animIndex = 0;

            for(cJSON * animNode = animations->child; animNode != NULL; animNode = animNode->next)
            {
                Animation * currentAnimation = &animGroup->animations[animIndex++];
                //this is not used
                char * animFileName = cJSON_GetObjectItem(animNode, "filename")->valuestring;

                currentAnimation->numFrames = cJSON_GetObjectItem(animNode, "numframes")->valueint;

                currentAnimation->lengthSeconds = cJSON_GetObjectItem(animNode, "lengthseconds")->valuedouble;

                cJSON * frames = cJSON_GetObjectItem(animNode, "frames");
                
                int frameCount = 0;

                for(cJSON * frameNode = frames->child; frameNode != NULL; frameNode = frameNode->next)
                {
                    int index1 = frameCount % 2;
                    int index2 = frameCount / 2;
                    
                    printf("framecount: %d index1: %d index2: %d\n", frameCount, index1, index2);

                    char * spritePath = frameNode->valuestring;
                    currentAnimation->frames[index1][index2] = getSpriteIndex(spritePath);
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
    if(loadAnimationData("../data/heroineAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load heroine animation data!\n");
        return false;
    }
    return true;
}