#ifndef SOUND_H_
#define SOUND_H_

bool initSound(void);
void setSoundVolume(int vol);
void setMusicVolume(int vol);
void loadMusic(char *fileName);
void playMusic(bool loop);
void pauseMusic(bool pause);
void playSound(int id, int channel);
void playSoundLoop(int id, int channel);
void stopChannel(int channel);

#endif