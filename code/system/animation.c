#include "../common.h"
#include "../json/cJSON.h"

#include "../system/memory.h"

#include "utils.h"
#include "asset.h"
#include "atlas.h"
#include "animation.h"

extern App app;

static char * animationTypes[] = 
{
    "Idle",
    "Run",
    "Melee",
    "Block",
    "Death",
    "LongBowShoot"
};

static char *animationSlots[] = 
{
    "Body",
    "Head",
    "Weapon",
    "Offhand"
};

static uint32_t getAnimationType(char *animType)
{
    for (int i = 0; i < POSE_COUNT; i++)
    {
        if(strcmp(animType, animationTypes[i]) == 0)
        {
            return i;
        }
    }
    return POSE_COUNT;
}

static uint32_t getAnimationSlot(char *animSlot)
{
    for (int i = 0; i < MAX_NUM_BODY_PARTS; i++)
    {
        if(strcmp(animSlot, animationSlots[i]) == 0)
        {
            return i;
        }
    }
    return MAX_NUM_BODY_PARTS;    
}

Animation *getAnimationByIndex(uint32_t index)
{
    return (Animation *)getAssetByIndex(AT_ANIMATION, index);
}

uint32_t getAnimationIndex(char * fileName)
{
    return getAssetIndex(AT_ANIMATION, fileName);
}

Animation *getAnimation(char * fileName)
{
    return (Animation *)getAsset(AT_ANIMATION, fileName);
}

AnimationGroup *getAnimationGroup(char *fileName)
{
    return (AnimationGroup *)getAsset(AT_ANIMATION_GROUP, fileName);
}

Animation *createAnimation(char * fileName, int * new)
{
    return (Animation *)createAsset(AT_ANIMATION, fileName, new);
}

AnimationGroup *createAnimationGroup(char *fileName, int *new)
{
    return (AnimationGroup *)createAsset(AT_ANIMATION_GROUP, fileName, new);
}

bool addAnimationGroupToAnimationController(char *fileName, AnimationController *controller, char *animationSlot)
{
    if(controller->numBodyParts == MAX_NUM_BODY_PARTS)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Can't add more body parts!\n");
        return false;
    }
    
    //first get the animation type from slot
    uint32_t animSlot = getAnimationSlot(animationSlot);
    //if we don't already have this animation slot, we will append the animations to the end
    //otherwise we will replace the animations for that slot
    uint32_t bodyPart = controller->numBodyParts;
    //find the correct slot
    for (int i = 0; i < controller->numBodyParts; i++)
    {
        //check the type of first animation
        int animIndex = controller->animationIndices[i][0];
        Animation *anim = getAnimationByIndex(animIndex);
        if (anim->animationSlot == animSlot)
        {
            bodyPart = i;
        }
    }

    //if the controller doesn't have animations for this body part,
    //we need to increment the body part count.
    if (bodyPart == controller->numBodyParts)
    {
        controller->numBodyParts++;
    }

    //get the animation group which just holds the indices to the animations of this group
    //now we know which slot they should go.
    AnimationGroup *animGroup = getAnimationGroup(fileName);

    if(animGroup == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, 
                       "Why the fuck does %s animation group not exist?\n", fileName);
        return false;
    }
    
    for(int i = 0; i < MAX_NUM_ANIMATIONS_PER_BODY_PART; i++)
    {
        controller->animationIndices[bodyPart][i] = animGroup->animationIndices[i];
        
    }

    return true;
}

//should create animationgroup and individual animation assets
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
        int newAnimGroup;
        char *groupFileName = cJSON_GetObjectItem(node, "filename")->valuestring;
        AnimationGroup *animGroup = createAnimationGroup(groupFileName, &newAnimGroup);
        int animCount = 0;

        if (newAnimGroup == true)
        {
            int animGroupNameLength = strlen(groupFileName);
            STRNCPY(animGroup->fileName.data, groupFileName, animGroupNameLength);
            cJSON * animations = cJSON_GetObjectItem(node, "animations");
            uint32_t numBodyParts = cJSON_GetObjectItem(node, "numbodyparts")->valueint;
            uint32_t animationSlot = getAnimationSlot(cJSON_GetObjectItem(node, "slot")->valuestring);
            for(cJSON * animNode = animations->child; animNode != NULL; animNode = animNode->next)
            {
                int newAnim;
                char *fileName = cJSON_GetObjectItem(animNode, "filename")->valuestring;
                Animation * currentAnimation = createAnimation(fileName, &newAnim);

                if(newAnim == true)
                {
                    STRNCPY(currentAnimation->fileName.data, fileName, strlen(fileName));
                    uint32_t animIndex = getAnimationIndex(fileName);
                    if(animIndex != MAX_NUM_ANIMATIONS)
                    {
                        animGroup->animationIndices[animCount++] = animIndex;
                    }
                    currentAnimation->numSprites = cJSON_GetObjectItem(animNode, "numframes")->valueint;
                    currentAnimation->lengthSeconds = cJSON_GetObjectItem(animNode, "lengthseconds")->valuedouble;
                    char *type = cJSON_GetObjectItem(animNode, "animationtype")->valuestring;
                    currentAnimation->animationType = getAnimationType(type);
                    currentAnimation->animationSlot = animationSlot;
                    cJSON * frames = cJSON_GetObjectItem(animNode, "frames");

                    int frameCount = 0;

                    for(cJSON * frameNode = frames->child; frameNode != NULL; frameNode = frameNode->next)
                    {
                        char * spritePath = frameNode->valuestring;
                        currentAnimation->animationSprites[frameCount] = *(getSprite(spritePath));
                        frameCount++;
                    }
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
    if(loadAnimationData("../data/clothesAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load clothes animation data!\n");
        return false;
    }
    
    if(loadAnimationData("../data/steel_armorAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load clothes animation data!\n");
        return false;
    }

    if(loadAnimationData("../data/head_longAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load head_long animation data!\n");
        return false;
    }

    if(loadAnimationData("../data/longswordAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load longsword animation data!\n");
        return false;
    }

    if(loadAnimationData("../data/bucklerAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load buckler animation data!\n");
        return false;
    }

    if(loadAnimationData("../data/shieldAnimation.json") == false)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_CATEGORY_ERROR, "Can't load shield animation data!\n");
        return false;
    }

    return true;
}