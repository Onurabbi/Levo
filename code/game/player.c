#include "../common.h"

#include "../system/atlas.h"
#include "../system/input.h"
#include "../system/utils.h"
#include "../system/sound.h"
#include "../system/ray.h"
#include "../system/vector.h"
#include "../system/animation.h"

#include "dungeon.h"
#include "astar.h"
#include "entityFactory.h"
#include "player.h"
#include "entity.h"
#include "actor.h"
#include "weapon.h"

extern App app;
extern Dungeon dungeon;

void updatePlayer(Entity * e)
{
    Actor * player = (Actor *)e->data;
    Input * input = &app.input;

    ActorController controller;
    controller.attack = false;
    controller.changeFacingDirection = false;

    if (input->mouse.buttons[1] == true)
    {
        controller.attack = true;
    }

    e->currentTile = getTileAtRowColLayerRaw(e->p.y, e->p.x, 1);
    e->currentTile->flags = 0x1;

    float dx = 0.0f;
    float dy = 0.0f;
    float deltaX, deltaY;

    if (input->keyboardState[SDL_SCANCODE_W] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)
        {
            controller.facing = FACING_RIGHT_UP;
            controller.changeFacingDirection = true;
            dx = 1.0f;
            dy = -1.0f;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            controller.facing = FACING_LEFT_UP;
            controller.changeFacingDirection = true;
            dx = -1.0f;
            dy = -1.0f;
        }
        else
        {
            controller.facing = FACING_UP;
            controller.changeFacingDirection = true;
            dx = 0.0f;
            dy = -1.0f;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_S] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)
        {
            controller.facing = FACING_RIGHT_DOWN;
            controller.changeFacingDirection = true;
            dy = 1.0f;
            dx = 1.0f;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            controller.facing = FACING_LEFT_DOWN;
            controller.changeFacingDirection = true;
            dy = 1.0f;
            dx = -1.0f;
        }
        else
        {
            controller.facing = FACING_DOWN;
            controller.changeFacingDirection = true;
            dy = 1.0f;
            dx = 0.0f;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_D] == 1)
    {
        controller.facing = FACING_RIGHT;
        controller.changeFacingDirection = true;
        dx = 1.0f;
        dy = 0.0f;
    }
    else if (input->keyboardState[SDL_SCANCODE_A] == 1)
    {
        controller.facing = FACING_LEFT;
        controller.changeFacingDirection = true;
        dx = -1.0f;
        dy = 0.0f;
    }

    deltaX = (dx + dy) / 2.0f;
    deltaY = (dy - dx) / 2.0f;
    
    if(canActorMove(player))
    {
        moveEntity(e, deltaX, deltaY);
    }

    updateActor(player, controller);

    dungeon.camera.x = e->p.x - dungeon.camera.w / 2;
    dungeon.camera.y = e->p.y - dungeon.camera.h / 2;
}