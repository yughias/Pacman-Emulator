#include "hardware.h"
#include <SDL_MAINLOOP.h>

uint8_t* AUDIO_ROM;
uint32_t VOICE_ACCUMULATOR[3];
uint64_t audioCycles;

SDL_AudioSpec audioSpec;
SDL_AudioDeviceID audioDev;

void initAudioData(){
    SDL_memset(&audioSpec, 0, sizeof(audioSpec));
    audioSpec.freq = AUDIO_FREQUENCY;
    audioSpec.format = AUDIO_U16;
    audioSpec.channels = 1;
    audioDev = SDL_OpenAudioDevice(NULL, 0, &audioSpec, &audioSpec, 0);
    SDL_PauseAudioDevice(audioDev, 0);
    AUDIO_ROM = malloc(sizeof(uint8_t)*AUDIO_ROM_SIZE);
    memset(VOICE_ACCUMULATOR, 0, sizeof(uint32_t)*3);
    loadROM("ROM/82s126.1m", 256, AUDIO_ROM);
    loadROM("ROM/82s126.3m", 256, AUDIO_ROM+256);
}

void freeAudioData(){
    free(AUDIO_ROM);
    SDL_CloseAudioDevice(audioDev);
}

void generateAudioSample(){
    uint32_t frequency;
    uint8_t waveform;
    uint8_t idx;
    uint8_t sample;
    uint8_t volume;

    uint16_t deviceSample = 0;

    if(!SOUND_ENABLED){
        SDL_QueueAudio(audioDev, &deviceSample, 2);
        return;
    }

    frequency = 0;
    for(int i = 0; i < 5; i++)
        frequency |= (VOICE1_FREQ[i] & 0xF) << (i*4);
    VOICE_ACCUMULATOR[0] += frequency;
    VOICE_ACCUMULATOR[0] &= 0xFFFFF;
    waveform = SOUND_VOICE1[5] & 0b111;
    idx = VOICE_ACCUMULATOR[0] >> 15;
    sample = AUDIO_ROM[32*waveform+idx];
    volume = VOICE1_VOLUME & 0xf;
    deviceSample += sample*volume;

    frequency = 0;
    for(int i = 0; i < 4; i++)
        frequency |= (VOICE2_FREQ_VOL[i] & 0xF) << (i*4+4);
    VOICE_ACCUMULATOR[1] += frequency;
    VOICE_ACCUMULATOR[1] &= 0xFFFFF;
    waveform = SOUND_VOICE2[4] & 0b111;
    idx = VOICE_ACCUMULATOR[1] >> 15;
    sample = AUDIO_ROM[32*waveform+idx];
    volume = VOICE2_FREQ_VOL[4] & 0xf;
    deviceSample += sample*volume;

    frequency = 0;
    for(int i = 0; i < 4; i++)
        frequency |= (VOICE3_FREQ_VOL[i] & 0xF) << (i*4+4);
    VOICE_ACCUMULATOR[2] += frequency;
    VOICE_ACCUMULATOR[2] &= 0xFFFFF;
    waveform = SOUND_VOICE3[4] & 0b111;
    idx = VOICE_ACCUMULATOR[2] >> 15;
    sample = AUDIO_ROM[32*waveform+idx];
    volume = VOICE3_FREQ_VOL[4] & 0xf;
    deviceSample += sample*volume;

    SDL_QueueAudio(audioDev, &deviceSample, 2);
}