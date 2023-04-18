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
    
    e->currentTile = getTileAtRowColLayerRaw(e->p.y, e->p.x, 1);
    e->currentTile->flags = 0x1;

    Entity *player = dungeon.player;
    float sqDist = sqDistance(e->p, player->p);

    if (sqDist < 100.0f)
    {
        Vec2f target = findPath(e->p, player->p);
        if (target.x != INFINITY && target.y != INFINITY)
        {
            debugDrawRect(target, 1.0f, 1.0f, 0, 255, 0, 0);
            MapTile *tile = getTileAtRowColLayerRaw(target.y, target.x, 1);
            Vec2f dir = vectorNormalize(vectorSubtract(target, e->p));
            if(canActorMove(enemy))
            {
                moveEntityRaw(e, dir.x, dir.y);
            }
        }
    }

    ActorController controller;
    controller.changeFacingDirection = true;
    controller.facing = FACING_LEFT;
    controller.attack = false;
    
    updateActor(enemy, controller);
    //move to player if found path
}