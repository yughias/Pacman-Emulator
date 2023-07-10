#include "frontend.h"
#include "gameState.h"
#include "hardware.h"

bool         emulationStopped = false;
bool         soundMute        = false;
unsigned int emulationSpeed   = 1;
unsigned int volumeMultiplier = 20;

bool         usingShader = false; 
Shader       crtShader;

void printInfo(){
    printf("PAC-MAN EMULATOR\n");
    printf("MADE BY YUGHIAS!\n\n");
    printf("ARCADE COMMANDS\n");
    printf("1:\t\tSTART ONE PLAYER MODE\n");
    printf("2:\t\tSTART TWO PLAYER MODE\n");
    printf("6:\t\tCHANGE COIN PER GAME\n");
    printf("7:\t\tCHANGE LIVES PER GAME\n");
    printf("8:\t\tCHANGE BONUS SCORE FOR EXTRA LIFE\n");
    printf("9:\t\tCHANGE DIFFICULTY\n");
    printf("0:\t\tCHANGE GHOST NAMES\n");
    printf("ARROW KEYS:\tPLAYER ONE JOYSTICK\n");
    printf("RIGHT SHIFT:\tINSERT COIN\n");
    printf("\n");
    printf("HOTKEYS\n");
    printf("CAPS:\t\tENABLE/DISABLE TURBO MODE\n");
    printf("F1:\t\tMUTE/UNMUTE\n");
    printf("F2:\t\tDECREASE VOLUME\n");
    printf("F3:\t\tINCREASE VOLUME\n");
    printf("F4:\t\tSTOP/RESUME EMULATION\n");
    printf("F5:\t\tRESET EMULATION\n");
    printf("F6:\t\tSAVE STATE\n");
    printf("F7:\t\tLOAD STATE\n");
    printf("\n");
    printf("LOG\n");
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

            case SDLK_RETURN:
                if(keyState[SDL_SCANCODE_RALT])
                    fullScreen();
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

            case SDLK_F4:
            emulationStopped = !emulationStopped;
            break;

            case SDLK_F5:
            initCPU();
            break;

            case SDLK_F6:
            saveState();
            break;

            case SDLK_F7:
            loadState();
            break;

            case SDLK_F8:
            if(usingShader){
                setScaleMode(NEAREST);
                noGlobalShader();
            } else {
                setScaleMode(LINEAR);
                setGlobalShader(crtShader);
            }
            usingShader = !usingShader;
            break;
        }
    }
}