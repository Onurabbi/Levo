#include "../common.h"

#include "../system/input.h"
#include "../system/utils.h"

#include "entityFactory.h"
#include "player.h"
#include "entity.h"
#include "actor.h"

extern App app;
extern Dungeon dungeon;

static Entity * projectile = NULL;
static Vec2f projectiledP;

typedef struct 
{

}AnimationUpdate;

void updateActor(Entity * player)
{
    
}

void updatePlayer(Entity * player)
{
    Actor * playerActor = (Actor *)player->data;
    playerActor->shooting = false;

    Input * input = &app.input;

    float dx = 0.0f;
    float dy = 0.0f;

    float deltaX, deltaY;

    if(input->keyboardState[SDL_SCANCODE_W] == 1)
    {
        dy = -1.0f;
    }
    if(input->keyboardState[SDL_SCANCODE_S] == 1)
    {
        dy = 1.0f;
    }
    if(input->keyboardState[SDL_SCANCODE_D] == 1)
    {
        dx = 1.0f;
    }
    if(input->keyboardState[SDL_SCANCODE_A] == 1)
    {
        dx = -1.0f;
    }
    
    deltaX = (dx + dy) / 2.0f;
    deltaY = (dy - dx) / 2.0f;

    moveEntity(player, deltaX, deltaY);

    //set player sprite
    player->sprite = getSpriteToShow(playerActor);

    if(projectile)
    {
        if(projectile->p.x > 0 && projectile->p.x < MAP_WIDTH && projectile->p.y > 0 && projectile->p.y < MAP_HEIGHT)
        {
            projectile->p.x += projectiledP.x;
            projectile->p.y += projectiledP.y;
        }
    }

    Entity * playerTargetEntity = &dungeon.entities[app.entityOverCursorIndex];

    if(input->mouse.buttons[1] == 1)
    {
        playerActor->shooting = true;
        printf("shooting\n");
    }

    if(input->mouse.buttons[2] == 1)
    {
        Vec2f barrelPos = toCartesian(app.mouseScreenPosition);
        Entity * newBarrel = initEntity("Barrel");
        newBarrel->p.x = barrelPos.x;
        newBarrel->p.y = barrelPos.y;
        printf("created new barrel at: %f %f\n", barrelPos.x, barrelPos.y);
    }

    //do animations
    
    dungeon.camera.x = player->p.x - dungeon.camera.w / 2;
    dungeon.camera.y = player->p.y - dungeon.camera.h / 2;

    clearInput(&app.input);
}