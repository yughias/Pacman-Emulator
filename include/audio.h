#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL_MAINLOOP.h>

#define AUDIO_ROM_SIZE 512
#define AUDIO_FREQUENCY 96000
#define VOLUME_MULTIPLIER 20

void initAudioData();
void freeAudioData();
void unpauseAudio();
uint16_t generateAudioSample();

#endif