#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include <stdbool.h>
#include <SDL2/SDL.h>

#define VOLUME_MULTIPLIER_LIMIT 95

extern bool         emulationStopped;
extern bool         soundMute;
extern unsigned int emulationSpeed;
extern unsigned int volumeMultiplier;

void printInfo();
void updateHotKeys(const Uint8*);

#endif