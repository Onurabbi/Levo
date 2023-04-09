#include <SDL2/SDL_mixer.h>

#include "../common.h"

#include "sound.h"

static Mix_Chunk * sounds[SND_MAX];
static Mix_Music * music;

static bool loadSounds(void)
{
	sounds[SND_GUI] = Mix_LoadWAV("../assets/sounds/107152__bubaproducer__button-5.ogg");
	sounds[SND_WALK] = Mix_LoadWAV("../assets/sounds/166506__yoyodaman234__concrete-footstep-4.ogg");
	sounds[SND_GAME_OVER] = Mix_LoadWAV("../assets/sounds/255729__manholo__inception-stab-l.ogg");
	sounds[SND_MISS] = Mix_LoadWAV("../assets/sounds/160500__timmy-h123__whoosh-01.ogg");
	sounds[SND_HIT] = Mix_LoadWAV("../assets/sounds/573376__johnloser__cyber-punch-01.ogg");
	sounds[SND_ANTIDOTE] = Mix_LoadWAV("../assets/sounds/56271__q-k__water-gulp.ogg");
	sounds[SND_STAIRS] = Mix_LoadWAV("../assets/sounds/442770__qubodup__walk-down-stairs.ogg");
	sounds[SND_PICKUP_ITEM] = Mix_LoadWAV("../assets/sounds/571629__ugila__item-pickup.ogg");
	sounds[SND_HEALTH_KIT] = Mix_LoadWAV("../assets/sounds/565267__ninjasharkstudios__duct-tape-short-1.ogg");
	sounds[SND_INVENTORY] = Mix_LoadWAV("../assets/sounds/442858__qubodup__page-flip.ogg");
	sounds[SND_LEVEL_UP] = Mix_LoadWAV("../assets/sounds/368651__jofae__game-powerup.mp3");
	sounds[SND_DOOR_OPEN] = Mix_LoadWAV("../assets/sounds/317529__theshaggyfreak__wooden-door-open3.mp3");
	sounds[SND_DOOR_UNLOCKED] = Mix_LoadWAV("../assets/sounds/511491__andersmmg__triple-beep.ogg");
	sounds[SND_DOOR_LOCKED] = Mix_LoadWAV("../assets/sounds/572936__bloodpixelhero__error.ogg");
	sounds[SND_EQUIP] = Mix_LoadWAV("../assets/sounds/525017__preyk__plug-getting-connected-to-wall-socket.ogg");
	sounds[SND_TRASH] = Mix_LoadWAV("../assets/sounds/82370__gynation__paper-crush-medium-1.ogg");
	sounds[SND_GAME_COMPLETE] = Mix_LoadWAV("../assets/sounds/413203__joepayne__clean-and-pompous-fanfare-trumpet.mp3");
	sounds[SND_SQUEAK_1] = Mix_LoadWAV("../assets/sounds/257773__xtrgamr__toy-squeak.ogg");
	sounds[SND_SQUEAK_2] = Mix_LoadWAV("../assets/sounds/343937__reitanna__pathetic-squeak.ogg");
	sounds[SND_SQUEAK_3] = Mix_LoadWAV("../assets/sounds/343970__reitanna__squeak2.ogg");
	sounds[SND_SQUEAK_4] = Mix_LoadWAV("../assets/sounds/428114__higgs01__squeakfinal.ogg");

	return true;
}

bool initSound(void)
{
	memset(sounds, 0, sizeof(Mix_Chunk *) * SND_MAX);
	
	music = NULL;

	return loadSounds();
}

void setSoundVolume(int vol)
{
	Mix_Volume(-1, vol);
}

void setMusicVolume(int vol)
{
	Mix_VolumeMusic(vol);
}

void loadMusic(char * fileName)
{
	if(music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(fileName);

	if(music == NULL)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, "Music at %s couldn't be loaded. Error: %s\n", fileName, SDL_GetError());
		return;
	}
}

void playMusic(bool loop)
{
	Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void pauseMusic(bool pause)
{
	if (pause)
	{
		Mix_PauseMusic();
	}
	else
	{
		Mix_ResumeMusic();
	}
}

void playSound(int id, int channel)
{
	Mix_PlayChannel(channel, sounds[id], 0);
}

void playSoundLoop(int id, int channel)
{
	Mix_PlayChannel(channel, sounds[id], -1);
}

void stopChannel(int channel)
{
	Mix_HaltChannel(channel);
}

void haltMusic(void)
{
	Mix_HaltMusic();
}