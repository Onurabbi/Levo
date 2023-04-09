#include "../common.h"

#include "../system/utils.h"
#include "../system/animation.h"
#include "../system/vector.h"
#include "../system/sound.h"

#include "collision.h"
#include "entity.h"
#include "actor.h"
#include "weapon.h"

extern Dungeon dungeon;
extern App app;

void swingWeapon(Weapon *weapon)
{
    weapon->attackTimer += DELTA_TIME;
    if (weapon->attackTimer >= weapon->attackCooldown)
    {
        weapon->attackTimer = 0.0f;
        weapon->damageDealt = false;
    }
    else
    {
        if ((weapon->attackTimer >= weapon->hitDelay) &&
            (weapon->damageDealt == false))
        {
            weapon->damageDealt = true;
            playSound(weapon->hitSound, -1);

            Entity *owner = weapon->owner;
            Entity *equipper = weapon->equipper;
            Actor *actor = (Actor *)equipper->data;

            SimulationRegion simRegion = getSimulationRegion();
            float sqWeaponLength = weapon->reach * weapon->reach;
            for(int i = 0; i < simRegion.totalNumEntities; i++)
            {
                uint32_t entityIndex = simRegion.entities[i];
                if ((entityIndex != equipper->entityIndex) && (entityIndex != owner->entityIndex))
                {
                    Entity *entity = &dungeon.entities[entityIndex];
                    Vec2f normalDir = vectorNormalize(vectorSubtract(entity->p, equipper->p));

                    if(sqDistance(equipper->p, entity->p) <= sqWeaponLength)
                    {
                            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, 
                            "entity within weapon reach\n");
                        Vec2f frontVector = getEntityFrontVector(actor->facing);

                        float dot = vectorDotProduct(normalDir, frontVector);
                        
                        switch(entity->entityType)
                        {
                            case ET_BARREL:
                                Actor * barrel = (Actor *)entity->data;
                                if(dot > 0.0f)
                                {
                                    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, 
                                                    "Hit barrel entity with id: %d\n", entity->entityIndex);
                                    takeDamage(barrel, actor, 1);
                                }
                                break;
                            default:
                                printf("Non barrel entity type hit\n");
                                break;
                        }
                    }
                }
            }
        }
    }
}

void equipWeapon(Weapon * weapon, Actor *equipper)
{
    //player stepped on us.
    SDL_assert(equipper);
    //stop showing weapon sprite.
    Entity *owner = weapon->owner;
    owner->entitySprites.drawableCount = 0;
    //memset(&owner->entitySprites, 0, sizeof(EntityVisibleSprites));
    BIT_CLEAR(owner->flags, ENTITY_IS_VISIBLE_BIT);

    weapon->equipper = equipper->owner;
    equipper->weapon = weapon;

    //add the entity animation to equipper animations
    if (addAnimationGroupToAnimationController("gfx/animations/longsword.animGroup", equipper->animationController, "Weapon") == true)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                        "Player equipped the sword\n");
    }
    else
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                        "Something went wrong!\n");
    }
}

void updateWeapon(Entity *e)
{
    Weapon *weapon = (Weapon *)e->data;
    SimulationRegion simRegion = getSimulationRegion();
    Input *input = &app.input;
    //if not equipped, check if someone is trying to equip us.
    if (weapon->equipper == NULL)
    {
        //only player can equip us
        Entity *player = &dungeon.entities[0];
        if (circleCircleCollision(e->p, e->width, player->p, player->width) == true)
        {
            if (input->mouse.buttons[2] == 1)
            {
                equipWeapon(weapon, (Actor *)player->data);
            }
        }
    }
    else
    {
        //weapon follows equipper
        e->p = weapon->equipper->p;
        Actor *actor = (Actor *)weapon->equipper->data;
        //if equipped, then handle weapon swinging.
        if (isActorAttacking(actor))
        {
            swingWeapon(weapon);
        }
    }
}
