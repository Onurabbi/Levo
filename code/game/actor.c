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
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "killing actor %d\n", actor->actorIndex);
        die(owner);
    }
}

bool isActorAttacking(Actor *actor)
{
    AnimationController *animController = actor->animationController;
    return (animController->animationState == POSE_MELEE);    
}

bool canActorMove(Actor *actor)
{
    AnimationController *animController = actor->animationController;
    return isAnimationInMovableState(animController);
}

static int getActorFacingDirection(ActorController controller, uint32_t oldFacing)
{
    int newFacing;
    if (controller.changeFacingDirection == true)
    {
        newFacing = controller.facing;
    }
    else
    {
        newFacing = oldFacing; 
    }
    return newFacing;
}

void updateActor(Actor *actor, ActorController controller)
{
    Entity *owner = actor->owner;
    AnimationController *animController = actor->animationController;
    if (controller.attack == true)
    {
        if (animController->animationState != POSE_MELEE)
        {
            animController->animationState = POSE_MELEE;
            animController->animStateChange = true;
        }
    }

    if (controller.wait == true)
    {
        if (animController->animationState != POSE_IDLE)
        {
            animController->animationState = POSE_IDLE;
            animController->animStateChange = true;
            actor->dP.x = 0.0f;
            actor->dP.y = 0.0f;
        }
    }

    actor->facing = getActorFacingDirection(controller, actor->facing);

    ActorMovement movement;
    movement.dP = actor->dP;
    movement.facing = actor->facing;

    updateAnimation(&owner->entitySprites, animController, movement);
}