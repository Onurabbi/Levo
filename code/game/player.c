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

    player->controller.attack = false;
    player->controller.changeFacingDirection = false;

    if (input->mouse.buttons[1] == true)
    {
        player->controller.attack = true;
    }
    
    float dx = 0.0f;
    float dy = 0.0f;
    float deltaX, deltaY;

    if (input->keyboardState[SDL_SCANCODE_W] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)
        {
            player->controller.facing = FACING_RIGHT_UP;
            player->controller.changeFacingDirection = true;
            dx = 1.0f;
            dy = -1.0f;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            player->controller.facing = FACING_LEFT_UP;
            player->controller.changeFacingDirection = true;
            dx = -1.0f;
            dy = -1.0f;
        }
        else
        {
            player->controller.facing = FACING_UP;
            player->controller.changeFacingDirection = true;
            dx = 0.0f;
            dy = -1.0f;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_S] == 1)
    {
        if (input->keyboardState[SDL_SCANCODE_D] == 1)
        {
            player->controller.facing = FACING_RIGHT_DOWN;
            player->controller.changeFacingDirection = true;
            dy = 1.0f;
            dx = 1.0f;
        }
        else if (input->keyboardState[SDL_SCANCODE_A] == 1)
        {
            player->controller.facing = FACING_LEFT_DOWN;
            player->controller.changeFacingDirection = true;
            dy = 1.0f;
            dx = -1.0f;
        }
        else
        {
            player->controller.facing = FACING_DOWN;
            player->controller.changeFacingDirection = true;
            dy = 1.0f;
            dx = 0.0f;
        }
    }
    else if (input->keyboardState[SDL_SCANCODE_D] == 1)
    {
        player->controller.facing = FACING_RIGHT;
        player->controller.changeFacingDirection = true;
        dx = 1.0f;
        dy = 0.0f;
    }
    else if (input->keyboardState[SDL_SCANCODE_A] == 1)
    {
        player->controller.facing = FACING_LEFT;
        player->controller.changeFacingDirection = true;
        dx = -1.0f;
        dy = 0.0f;
    }

    deltaX = (dx + dy) / 2.0f;
    deltaY = (dy - dx) / 2.0f;
    
    if(canActorMove(player))
    {
        moveEntity(e, deltaX, deltaY);
    }

    updateActor(player, player->controller);

    dungeon.camera.x = e->p.x - dungeon.camera.w / 2;
    dungeon.camera.y = e->p.y - dungeon.camera.h / 2;
}