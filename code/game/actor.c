#include "../common.h"

#include "actor.h"

extern Dungeon dungeon;
extern App app;

Actor * getNewActor(void)
{
    SDL_assert(dungeon.numActors < MAX_NUM_ACTORS);
    return &dungeon.actors[dungeon.numActors++];
}