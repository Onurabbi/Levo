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

void addAnimationToEntityAnimGroup(Entity *e, AnimationGroup *animGroup, char *path)
{
    AnimationGroup *toAdd = getAnimationGroup(path);
    int slot = e->entitySprites.drawableCount;
    for(int i = 0; i < MAX_NUM_ANIMATIONS_PER_GROUP; i++)
    {
        Animation *anim = &animGroup->animations[i];
        for(int j = 0; j < anim->numFrames; j++)
        {
            anim->frames[slot + animGroup->numBodyParts *j] = toAdd->animations[i].frames[j];
        }
    }
    e->entitySprites.sprites[slot] = getSprite("gfx/entities/longswordLeftIdle1.png");
    e->entitySprites.drawableCount++;
}

static bool loadAnimationData(char * filePath, bool character)
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
            
            if(character == true)
            {
                animGroup->maxNumBodyParts = MAX_DRAWABLES_PER_ENTITY;
                animGroup->numBodyParts = cJSON_GetObjectItem(node, "numbodyparts")->valueint;
            }
            else
            {
                animGroup->numBodyParts = cJSON_GetObjectItem(node, "numbodyparts")->valueint;
                animGroup->maxNumBodyParts = animGroup->numBodyParts;
            }

            cJSON * animations = cJSON_GetObjectItem(node, "animations");

            int animIndex = 0;

            for(cJSON * animNode = animations->child; animNode != NULL; animNode = animNode->next)
            {
                Animation * currentAnimation = &animGroup->animations[animIndex++];
                //this is not used
                currentAnimation->numFrames = cJSON_GetObjectItem(animNode, "numframes")->valueint;
                currentAnimation->frames = allocatePermanentMemory(currentAnimation->numFrames * animGroup->maxNumBodyParts * sizeof(uint32_t));

                char * animFileName = cJSON_GetObjectItem(animNode, "filename")->valuestring;

                currentAnimation->lengthSeconds = cJSON_GetObjectItem(animNode, "lengthseconds")->valuedouble;

                cJSON * frames = cJSON_GetObjectItem(animNode, "frames");

                int frameCount = 0;

                for(cJSON * frameNode = frames->child; frameNode != NULL; frameNode = frameNode->next)
                {
                    int bodyPart = frameCount % animGroup->numBodyParts;
                    int frameIndex = frameCount / animGroup->numBodyParts;

                    char * spritePath = frameNode->valuestring;
                    currentAnimation->frames[frameIndex * animGroup->maxNumBodyParts + bodyPart] = getSpriteIndex(spritePath);
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
    if(loadAnimationData("../data/heroineUnarmedAnimation.json", true) == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load heroine animation data!\n");
        return false;
    }

    if(loadAnimationData("../data/longswordAnimation.json", false) == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load longsword animation data!\n");
        return false;
    }

    return true;
}