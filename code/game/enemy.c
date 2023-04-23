#include "../common.h"

#include "../system/utils.h"
#include "../system/vector.h"
#include "../system/draw.h"

#include "dungeon.h"
#include "enemy.h"
#include "entity.h"
#include "actor.h"
#include "astar.h"

extern Dungeon dungeon;
extern App app;

void updateEnemy(Entity *e)
{
    Actor *enemy = (Actor *)e->data;
    enemy->dP.x = 0.0f;
    enemy->dP.y = 0.0f;

    Entity *player = dungeon.player;
    float sqDist = sqDistance(e->p, player->p);

    enemy->controller.changeFacingDirection = false;
    enemy->controller.attack = false;

    Vec2f dir = {INFINITY, INFINITY};

    if (sqDist < 2.0f)
    {
        dir = vectorNormalize(vectorSubtract(player->p, e->p));
    }
    else if (sqDist < 64.0f)
    {
        Vec2f target = findPath(e, player->p);
        if (target.x != INFINITY && target.y != INFINITY)
        {
            debugDrawRect(target, 1.0f, 1.0f, 0, 255, 0, 0);
            dir = vectorNormalize(vectorSubtract(target, e->p));
            if(canActorMove(enemy))
            {
                (void)moveEntityRaw(e, dir.x, dir.y);
            }
        }
    }
    else
    {
        //what to do here
    }

    //no enemy in range or no path found
    if (dir.x == INFINITY || dir.y == INFINITY)
    {
        enemy->controller.changeFacingDirection = false;
    }
    else
    {
        enemy->controller.changeFacingDirection = true;
        enemy->controller.facing = getFacingFromDirection(dir);
    }
    
    updateActor(enemy, enemy->controller);
    //move to player if found path
}