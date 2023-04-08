#include "../common.h"

#include "../system/atlas.h"
#include "../system/input.h"
#include "../system/utils.h"
#include "../system/sound.h"
#include "../system/ray.h"
#include "../system/vector.h"
#include "../system/animation.h"

#include "entityFactory.h"
#include "player.h"
#include "entity.h"
#include "actor.h"

extern App app;
extern Dungeon dungeon;

static Entity * projectile = NULL;
static Vec2f projectiledP;

void updatePlayer(Entity * player)
{
    Actor * playerActor = (Actor *)player->data;

    Input * input = &app.input;

    float dx = 0.0f;
    float dy = 0.0f;

    float deltaX, deltaY;

    if (input->keyboardState[SDL_SCANCODE_W] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)

        {
            dx = 1.0f;
            dy = -1.0f;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            dx = -1.0f;
            dy = -1.0f;
        }
        else
        {
            dx = 0.0f;
            dy = -1.0f;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_S] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)

        {
            dy = 1.0f;
            dx = 1.0f;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            dy = 1.0f;
            dx = -1.0f;
        }
        else
        {
            dy = 1.0f;
            dx = 0.0f;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_D] == 1)
    {
        dx = 1.0f;
        dy = 0.0f;
    }
    else if (input->keyboardState[SDL_SCANCODE_A] == 1)
    {
        dx = -1.0f;
    }

    deltaX = (dx + dy) / 2.0f;
    deltaY = (dy - dx) / 2.0f;
    
    if(playerActor->attacking == false)
    {
        moveEntity(player, deltaX, deltaY);
    }

    updateActor(playerActor);

    Entity * playerTargetEntity = &dungeon.entities[app.entityOverCursorIndex];
    Vec2f frontVector = vectorNormalize(getEntityFrontVector(playerActor->facing));
    Vec2f lineEnd = vectorAdd(vectorMultiply(frontVector, 2.0f), player->p);
    toIso(lineEnd.x, lineEnd.y, &app.lineX, &app.lineY);
    
    SimulationRegion simRegion = getSimulationRegion();

    if(input->mouse.buttons[1] == 1)
    {
        playerActor->attacking = true;
        playerActor->switchAnim = true;
        playSound(SND_HIT, 0);
        float sqWeaponLength = 4.0f;

        for(int i = 0; i < simRegion.totalNumEntities; i++)
        {
            if(i != player->entityIndex)
            {
                uint32_t entityIndex = simRegion.entities[i];
                Entity *e = &dungeon.entities[entityIndex];
                Vec2f normalDir = vectorNormalize(vectorSubtract(e->p, player->p));

                if(sqDistance(player->p, e->p) <= sqWeaponLength)
                {
                    float dot = vectorDotProduct(normalDir, frontVector);
                    
                    switch(e->entityType)
                    {
                        case ET_BARREL:
                            Actor * barrelActor = (Actor *)e->data;
                            if(dot > sqrt(2) * 0.5f)
                            {
                                takeDamage(barrelActor, playerActor, 1);
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

    dungeon.camera.x = player->p.x - dungeon.camera.w / 2;
    dungeon.camera.y = player->p.y - dungeon.camera.h / 2;

    clearInput(&app.input);
}