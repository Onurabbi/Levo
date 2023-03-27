#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "../common.h"

#include "job.h"
#include "asset.h"
#include "atlas.h"
#include "animation.h"
#include "draw.h"
#include "sound.h"
#include "init.h"
#include "text.h"
#include "widget.h"
#include "memory.h"

#include "../game/entity.h"
#include "../game/entityFactory.h"
#include "../game/dungeon.h"
#include "../game/title.h"

extern App app;

bool initSDL(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Couldn't initialize SDL Mixer\n");
		return false;
	}

	Mix_AllocateChannels(MAX_SND_CHANNELS);

	Mix_ReserveChannels(1);

    if(TTF_Init() < 0)
    {
        printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
        return false;
    }

    app.window = SDL_CreateWindow("BestGameEver", SDL_WINDOWPOS_UNDEFINED, 
                                                  SDL_WINDOWPOS_UNDEFINED, 
                                                  SCREEN_WIDTH, SCREEN_HEIGHT, 
                                                  SDL_WINDOW_SHOWN);
    
    if(!app.window)
    {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, 0);

    if(!app.renderer)
    {
        printf("Failed to create the renderer: %s\n", SDL_GetError());
        return false;
    }

    if(!initJobs())
    {
        printf("Failed to create the job system: %s\n", SDL_GetError());
        return false;
    }
    
    SDL_ShowCursor(SDL_DISABLE);
    
    return true;
}

bool initGameSystem(void)
{
    //SYSTEMS
    INIT_CHECK(initMemory, "initMemory");
    INIT_CHECK(initAssets, "initAssets");
    INIT_CHECK(initSprites, "initSprites");
#if 0
    INIT_CHECK(initAnimations, "initAnimations");
#endif
    INIT_CHECK(initDraw, "initDraw");
    INIT_CHECK(initSound, "initSound");
    INIT_CHECK(initFonts, "initFonts");
    INIT_CHECK(initWidgets, "initWidgets");

    //GAME
    INIT_CHECK(initEntityFactory, "initEntityFactory");
    INIT_CHECK(initEntities, "initEntities");
    //This will come last
    INIT_CHECK(initTitle, "initTitle");

    return true;
}

void cleanUp(void)
{
    Mix_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}