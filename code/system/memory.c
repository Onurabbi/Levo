#include "../common.h"

#include "memory.h"

GameMemory gameMemory;

bool initMemory(void)
{
    memset(&gameMemory, 0, sizeof(GameMemory));

    gameMemory.permanent.base = malloc(GAME_MEMORY_SIZE);
    if(gameMemory.permanent.base == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, 
                      "Can't allocate game memory. Exiting application!\n");
        return false;
    }
    
    memset(gameMemory.permanent.base, 0, GAME_MEMORY_SIZE);
    
    gameMemory.permanent.size = PERMANENT_MEMORY_SIZE;
    gameMemory.permanent.used = 0;

    gameMemory.transient.base = gameMemory.permanent.base + gameMemory.permanent.size;
    gameMemory.transient.size = TRANSIENT_MEMORY_SIZE;
    gameMemory.transient.used = 0;
    
    return true;
}

void * allocatePermanentMemory(size_t size)
{
    void * allocated = NULL;

    if(gameMemory.permanent.used + size <= gameMemory.permanent.size)
    {
        allocated = gameMemory.permanent.base + gameMemory.permanent.used;
        gameMemory.permanent.used += size;
    }
    return allocated;
}

void * allocateTransientMemory(size_t size)
{
    void * allocated = NULL;

    if(gameMemory.transient.used + size <= gameMemory.transient.size)
    {
        allocated = gameMemory.transient.base + gameMemory.transient.used;
        gameMemory.transient.used += size;
    }

    return allocated;
}

void resetTransientMemory(void)
{
    memset(gameMemory.transient.base, 0, gameMemory.transient.used);
    gameMemory.transient.used = 0;
}