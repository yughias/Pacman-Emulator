#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include <SDL_MAINLOOP.h>

#define VOLUME_MULTIPLIER_LIMIT 95

extern bool         emulationStopped;
extern bool         soundMute;
extern unsigned int emulationSpeed;
extern unsigned int volumeMultiplier;

extern Shader crtShader;
extern bool usingShader;

void printInfo();
void updateHotKeys(const Uint8*);

#endif