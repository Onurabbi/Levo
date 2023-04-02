#include "../common.h"

#include "../system/utils.h"
#include "../system/animation.h"
#include "../system/atlas.h"

#include "actor.h"

extern Dungeon dungeon;
extern App app;

static inline bool isActorMoving(Vec2f dP)
{
    return (sqAmplitude(dP) > epsilon);
}

static int getActorFacingDirection(int oldFacing)
{
    Input *input = &app.input;

    int newFacing;

    if (input->keyboardState[SDL_SCANCODE_W] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)
        {
            newFacing = FACING_RIGHT_UP;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            newFacing = FACING_LEFT_UP;
        }
        else
        {
            newFacing = FACING_UP;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_S] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)
        {
            newFacing = FACING_RIGHT_DOWN;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            newFacing = FACING_LEFT_DOWN;
        }
        else
        {
            newFacing = FACING_DOWN;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_D] == 1)
    {
        newFacing = FACING_RIGHT;
    }
    else if (input->keyboardState[SDL_SCANCODE_A] == 1)
    {
        newFacing = FACING_LEFT;
    }
    else
    {
        newFacing = oldFacing; 
    }

    return newFacing;

}

static int getAnimIndex(int facing, int pose)
{
    return (facing * POSE_COUNT + pose);
}

static Animation * getNewAnimation(Actor* actor)
{
    int animIndex = actor->animGroup->animationState;

    AnimationGroup * animGroup = actor->animGroup;

    if((actor->attacking == true) && (actor->switchAnim == true))
    {
        actor->switchAnim = false;

        animIndex = getAnimIndex(actor->facing, POSE_MELEE);
    }
    else if (actor->attacking != true)
    {
        bool moving = isActorMoving(actor->dP);
        int pose = moving ? POSE_RUN : POSE_IDLE;
        animIndex = getAnimIndex(actor->facing, pose);
    }

    actor->animGroup->animationState = animIndex;

    Animation * newAnimation = &actor->animGroup->animations[animIndex];
    
    return newAnimation;
}

EntityVisibleSprites getSpriteToShow(Actor * actor)
{
    EntityVisibleSprites result = {};

    actor->facing = getActorFacingDirection(actor->facing);

    Animation * currentAnimation = getNewAnimation(actor);

    int numFrames = currentAnimation->numFrames;

    double timePerFrame = currentAnimation->lengthSeconds / (double)numFrames;

    int frameIndex = (int)(currentAnimation->AnimTimer / timePerFrame);

    double animTime = currentAnimation->AnimTimer + 1.0/59.0;
 
    if(animTime > currentAnimation->lengthSeconds)
    {
        animTime = 0.0;

        if(actor->attacking)
        {
            actor->attacking = false;
        }
        else
        {
            frameIndex = ((int)(animTime / timePerFrame));
        }
    }

    currentAnimation->AnimTimer = animTime;

    SDL_assert(frameIndex >= 0 && frameIndex < currentAnimation->numFrames);

    result.head = getSpriteByIndex(currentAnimation->frames[HEAD][frameIndex]);
    result.body = getSpriteByIndex(currentAnimation->frames[BODY][frameIndex]);
    
    return result;
}

Actor * getNewActor(void)
{
    SDL_assert(dungeon.numActors < MAX_NUM_ACTORS);
    return &dungeon.actors[dungeon.numActors++];
}