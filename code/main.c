#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "common.h"

#include "system/draw.h"
#include "system/texture.h"
#include "system/atlas.h"
#include "system/animation.h"
#include "system/input.h"
#include "system/sound.h"
#include "system/init.h"
#include "system/text.h"
#include "system/widget.h"
#include "system/asset.h"
#include "system/utils.h"

#include "game/title.h"
#include "game/actor.h"
#include "game/entity.h"
#include "game/entityFactory.h"
#include "game/dungeon.h"
#include "game/collision.h"

App app;
Dungeon dungeon;

static void updateAndRenderInventory(void)
{
    drawText("INVENTORY", 0, 0, 255, 0, 0, TEXT_ALIGN_LEFT, 0);
}

void updateAndRender(double timeElapsedSeconds)
{
    handleInput(&app.input, timeElapsedSeconds);

    //TODO: could calling these before update functions be slow?
    //maybe I should separate update and renders
    prepareScene();

    switch(app.gameMode)
    {
        case GAME_MODE_TITLE:
            updateAndRenderTitle();
            break;
        case GAME_MODE_DUNGEON:
            updateAndRenderDungeon();
            break;
        case GAME_MODE_INVENTORY:
            updateAndRenderInventory();
            break;
        case GAME_MODE_MENU:
            updateAndRenderMenu();
            break;
        default:
            SDL_LogMessage(SDL_LOG_CATEGORY_ASSERT, SDL_LOG_PRIORITY_CRITICAL, "Invalid game mode!!!!!\n");
            SDL_assert(0);
            break;
    }
    presentScene();
}

int main(int argc, char* args[])
{
    memset(&app, 0, sizeof(App));
    app.showPointer = true;

    if(initSDL() == false)
    {
        return 1;
    }

    if(initGameSystem() == false)
    {
        printf("game system init fault\n");
        return 2;
    }

    app.run = true;

    uint64_t currentTicks = SDL_GetPerformanceCounter();
    uint64_t lastTicks    = 0;
    
    while(app.run == true)
    {
        lastTicks = currentTicks;
        currentTicks = SDL_GetPerformanceCounter();
        uint64_t elapsedTicks = currentTicks - lastTicks;

        double timeElapsedSeconds = ((double)elapsedTicks)/((double)(SDL_GetPerformanceFrequency()));

        updateAndRender(timeElapsedSeconds);
    }

    cleanUp();

    return 0;
}

