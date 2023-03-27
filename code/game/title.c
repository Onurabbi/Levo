#include "../common.h"

#include "../system/draw.h"
#include "../system/input.h"
#include "../system/sound.h"
#include "../system/text.h"
#include "../system/texture.h"

#include "dungeon.h"
#include "title.h"

extern App app;

static Texture * background;
static Texture * logo;

static double accumulator;

bool initTitle(void)
{
    background = loadTexture("../assets/textures/background.jpg");

    if(background == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Can't load background texture\n");
        return false;
    }

    logo = loadTexture("../assets/textures/logo.png");

    if(logo == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Can't load logo texture\n");
        return false;
    }

    loadMusic("../assets/music/Something is near.mp3");

    app.gameMode = GAME_MODE_TITLE;
    
    playMusic(true);

    return true;
}

static void update()
{
    Input * input = &app.input;

    if ((input->keyboardEvents[SDL_SCANCODE_SPACE] == 1) || (input->keyboardEvents[SDL_SCANCODE_RETURN] == 1))
    {
        clearInput(&app.input);
        pauseMusic(1);
        initDungeon();
    }

    if(input->keyboardEvents[SDL_SCANCODE_ESCAPE] == 1)
    {
        app.run = false;
    }
}

void updateAndRenderTitle(void)
{
    accumulator += app.input.secElapsed;
    
    while(accumulator > 1.0/61.0)
    {
        update();
        accumulator -= 1.0/59.0;
        if(accumulator < 0)
        {
            accumulator = 0;
        }
    }
    
    blit(background->texture, 0, 0, false);
    blit(logo->texture, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, true);
    app.fontScale = 1.5;
    drawText("Press space or return", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 200, 255, 255, 255, TEXT_ALIGN_CENTER, 0);
    app.fontScale = 1;
}
