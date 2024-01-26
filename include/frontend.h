#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include <SDL_MAINLOOP.h>

#define VOLUME_MULTIPLIER_LIMIT 48
#define MESSAGE_FRAME_TIME     60

extern const char coins_per_game_descriptor[4][32];
extern const char lives_per_game_descriptor[4][32];
extern const char bonus_score_descriptor[4][32];
extern const char difficulty_descriptor[2][32];
extern const char ghost_names_descriptor[2][32];

extern char         message[33];
extern unsigned int remaining_frames;

extern bool         startupScreen;
extern bool         emulationStopped;
extern bool         soundMute;
extern unsigned int emulationSpeed;
extern unsigned int volumeScaler;

extern Shader crtShader;
extern bool usingShader;

void updateHotKeys(const Uint8*);
void showText(const char*, int, int);
void updateFrontendMessage();
void setFrontendMessage(const char*);
void showStartupScreen();
void loadNextGame();

#endif