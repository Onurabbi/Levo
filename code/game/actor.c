#include "../common.h"

#include "../system/atlas.h"

#include "actor.h"

extern Dungeon dungeon;
extern App app;

static Animation * getNewAnimation(Actor* actor)
{
    //
    AnimationGroup * animGroup = actor->animGroup;

    int animIndex;

    if(actor->shooting)
    {
        
        animIndex = HEROINE_LEFT_BOW_SHOOT;
    }
    else
    {
        if(actor->dP.x < -epsilon)
        {
            animIndex = HEROINE_LEFT_RUN;
        }
        else if (actor->dP.x > epsilon)
        {
            animIndex = HEROINE_RIGHT_RUN;
        }
        else
        {
            if(actor->facing == FACING_LEFT)
            {
                animIndex = HEROINE_LEFT_IDLE;
            }
            else if(actor->facing == FACING_RIGHT)
            {
                animIndex = HEROINE_RIGHT_IDLE;
            }
        }
    }

    Animation * currentAnimation = &animGroup->animations[animIndex];
    return currentAnimation;
}

Sprite * getSpriteToShow(Actor * actor)
{
    Sprite * result = NULL;

    Animation * currentAnimation = getNewAnimation(actor);

    int numFrames = currentAnimation->numFrames;

    double timePerFrame = currentAnimation->lengthSeconds / (double)numFrames;

    double animTime = currentAnimation->AnimTimer + 1.0/59.0;

    if(animTime > currentAnimation->lengthSeconds)
    {
        animTime = 0.0;
    }

    currentAnimation->AnimTimer = animTime;

    int frameIndex = ((int)(animTime / timePerFrame));
    
    SDL_assert(frameIndex >= 0 && frameIndex < currentAnimation->numFrames);

    result = getSpriteByIndex(currentAnimation->frames[frameIndex]);

    //printf("sprite: %s\n", result->fileName);

    return result;
}

Actor * getNewActor(void)
{
    SDL_assert(dungeon.numActors < MAX_NUM_ACTORS);
    return &dungeon.actors[dungeon.numActors++];
}