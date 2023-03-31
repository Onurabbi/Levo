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

static double   accumulator;
static uint32_t ticks;
static char     fpsBuf[MAX_NAME_LENGTH];

void drawFPS(void)
{
    if(ticks == 0) sprintf(fpsBuf, "Ms per Frame: %.3f", 1.0f / app.input.secElapsed);

    ticks++;
    
    if(ticks == FPS_DRAW_TICKS) ticks = 0;

    app.fontScale = 1;

    drawText(fpsBuf, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 100, 255, 255, 255, TEXT_ALIGN_CENTER, 0);
}

static void updateInventory(void)
{

}

static void renderInventory(void)
{
    drawText("INVENTORY", 0, 0, 255, 0, 0, TEXT_ALIGN_LEFT, 0);
}

static void render(void)
{
    switch(app.gameMode)
    {
        case GAME_MODE_TITLE:
            renderTitle();
            break;
        case GAME_MODE_DUNGEON:
            renderDungeon();
            break;
        case GAME_MODE_INVENTORY:
            renderInventory();
            break;
        case GAME_MODE_MENU:
            renderMenu();
            break;
        default:
            SDL_LogMessage(SDL_LOG_CATEGORY_ASSERT, SDL_LOG_PRIORITY_CRITICAL, "Invalid game mode!!!!!\n");
            SDL_assert(0);
            break;
    }
}

static void update(void)
{
    accumulator += app.input.secElapsed;

    while (accumulator > 1.0/61.0)
    {
        double secElapsed = 1.0 / 59.0;

        switch(app.gameMode)
        {
            case GAME_MODE_TITLE:
                updateTitle();
                break;
            case GAME_MODE_DUNGEON:
                updateDungeon();
                break;
            case GAME_MODE_INVENTORY:
                updateInventory();
                break;
            case GAME_MODE_MENU:
                updateMenu(secElapsed);
                break;
            default:
                SDL_LogMessage(SDL_LOG_CATEGORY_ASSERT, SDL_LOG_PRIORITY_CRITICAL, "Invalid game mode!!!!!\n");
                SDL_assert(0);
                break;
        }

        accumulator -= secElapsed;
        
        if(accumulator < 0)
        {
            accumulator = 0;
        }
    }
}

void updateAndRender(double timeElapsedSeconds)
{
    handleInput(&app.input, timeElapsedSeconds);

    prepareScene();

    update();

    render();

    drawFPS();

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
    accumulator = 0.0;

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

