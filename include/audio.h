#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL_MAINLOOP.h>

#define AUDIO_ROM_SIZE 512
#define AUDIO_FREQUENCY 48000

void initAudioData();
void freeAudioData();
void calculateVolume();

#endif