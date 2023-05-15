#ifndef __AUDIO_H__
#define __AUDIO_H__

#define AUDIO_ROM_SIZE 512
#define AUDIO_FREQUENCY 96000

extern uint64_t audioCycles;

void initAudioData();
void freeAudioData();
void updateAudio();
void sendAudioData();

#endif