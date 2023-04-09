#include "../common.h"

#include "../system/utils.h"
#include "../system/animation.h"
#include "../system/atlas.h"

#include "entity.h"
#include "actor.h"

extern Dungeon dungeon;
extern App app;

void takeDamage(Actor *actor, Actor * attacker, uint32_t damage)
{
    Entity * owner = actor->owner;
    SDL_assert(owner);

    actor->health--;
    if(actor->health == 0)
    {
        printf("killing actor %d\n", actor->actorIndex);
        die(owner);
    }
}

bool isActorAttacking(Actor *actor)
{
    AnimationController *animController = actor->animationController;
    return (animController->animationState == POSE_MELEE);    
}

bool isActorMoving(Vec2f dP)
{
    return (sqAmplitude(dP) > epsilon);
}

bool canActorMove(Actor *actor)
{
    AnimationController *animController = actor->animationController;
    return ((animController->animationState != POSE_BLOCK) && 
            (animController->animationState != POSE_MELEE) &&
            (animController->animationState != POSE_BOW_SHOOT) &&
            (animController->animationState != POSE_DEATH));
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


static inline void getAnimationVisibleSprites(AnimationController *animController, EntityVisibleSprites *sprites, uint32_t frameIndex)
{
    sprites->drawableCount = 0;

    for(int i = 0; i < animController->numBodyParts; i++)
    {
        uint32_t animationIndex = animController->animationIndices[i][animController->currentAnimationIndex];
        Animation *animation = getAnimationByIndex(animationIndex);
        sprites->sprites[i] = &animation->animationSprites[frameIndex];
        sprites->drawableCount++;
    }
}

static uint32_t updateAnimationIndex(Actor* actor, AnimationController *animController)
{
    int animIndex = actor->animationController->currentAnimationIndex;

    if ((animController->animationState == POSE_MELEE) && (animController->animStateChange == true))
    {
        animController->animStateChange = false;
        animController->animationState = POSE_MELEE;
        animIndex = getAnimIndex(actor->facing, POSE_MELEE);
    }
    else if(animController->animationState != POSE_MELEE)
    {
        animController->animationState = isActorMoving(actor->dP) ? POSE_RUN : POSE_IDLE;
        animIndex = getAnimIndex(actor->facing, animController->animationState);
    }
    
    return animIndex;
}

static double updateAnimationTimer(Actor *actor, AnimationController *animController)
{
    double result = 0.0;
    result = animController->animTimer + 1.0/59.0;
    if(result > animController->animLengthInSeconds)
    {
        result = 0.0;

        if(animController->animationState == POSE_MELEE)
        {
            animController->animationState = POSE_IDLE;
        }
    }
    return result;
}

static void updateAnimation(Actor * actor)
{
    Entity * owner = actor->owner;
    AnimationController *animController = actor->animationController;
    
    actor->facing = getActorFacingDirection(actor->facing);
    //Animation controller is always initialised properly in entityfactory
    //update animation timer
    animController->animTimer = updateAnimationTimer(actor, animController);
    //update animation index
    uint32_t newAnimIndex = updateAnimationIndex(actor, animController);
    if (animController->currentAnimationIndex != newAnimIndex)
    {
        //update animation controller
        //any body part will do
        int newIndex = animController->animationIndices[0][newAnimIndex];
        Animation * newAnim = getAnimationByIndex(newIndex);
        animController->animLengthInSeconds = newAnim->lengthSeconds;
        animController->numFrames = newAnim->numSprites;
        animController->animTimer = 0.0;
    }
    animController->currentAnimationIndex = newAnimIndex;
    uint32_t numFrames = animController->numFrames;
    double timePerFrame = animController->animLengthInSeconds / (double)numFrames;
    uint32_t frameIndex = (int)(animController->animTimer / timePerFrame);
    getAnimationVisibleSprites(animController, &owner->entitySprites, frameIndex);
}

void updateActor(Actor *actor)
{
    Input *input = &app.input;
    if (input->mouse.buttons[1] == 1)
    {
        AnimationController *animController = actor->animationController;
        if (animController->animationState != POSE_MELEE)
        {
            animController->animationState = POSE_MELEE;
            animController->animStateChange = true;
        }
    }
    updateAnimation(actor);
}