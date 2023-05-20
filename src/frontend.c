#include <stdio.h>
#include "frontend.h"
#include "SDL_MAINLOOP.h"

unsigned int emulationSpeed = 1;
bool soundMute = false;
unsigned int volumeMultiplier = 20;

void printInfo(){
    printf("PAC-MAN EMULATOR\n");
    printf("MADE BY YUGHIAS!\n\n");
    printf("ARCADE COMMANDS\n");
    printf("1:\t\tSTART ONE PLAYER MODE\n");
    printf("2:\t\tSTART TWO PLAYERS MODE\n");
    printf("ARROW KEYS:\tPLAYER ONE JOYSTICK\n");
    printf("LEFT SHIFT:\tINSERT COIN\n");
}

void updateHotKeys(const Uint8* keyState){
    if(isKeyReleased){
        switch(keyReleased){
            case SDLK_CAPSLOCK:
            if(emulationSpeed == 1)
                emulationSpeed = 2;
            else
                emulationSpeed = 1;
            break;

            case SDLK_F1:
            soundMute = !soundMute;
            break;
        }
    }

    if(isKeyPressed){
        switch(keyPressed){
            case SDLK_F2:
            if(volumeMultiplier != 0)
                volumeMultiplier--;
            break;

            case SDLK_F3:
            if(volumeMultiplier < VOLUME_MULTIPLIER_LIMIT)
                volumeMultiplier++;
            break;
        }
    }
}