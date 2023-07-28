#include "frontend.h"
#include "romset.h"
#include "gameState.h"
#include "hardware.h"

const char coins_per_game_descriptor[4][32] = {
    "free play",
    "1 coin per game",
    "1 coin per 2 games",
    "2 coins per game"
};

const char lives_per_game_descriptor[4][32] = {
    "1 life",
    "2 lives",
    "3 lives",
    "5 lives"
};

const char bonus_score_descriptor[4][32] = {
    "10000 points",
    "15000 points",
    "20000 points",
    "no bonus"
};

const char difficulty_descriptor[2][32] = {
    "hard mode",
    "normal mode"
};

const char ghost_names_descriptor[2][32] = {
    "alternate ghost names",
    "normal ghost names"
};

char         message[33]      = "\0";
uint64_t     remaining_frames = 0;

bool         emulationStopped = false;
bool         soundMute        = false;
unsigned int emulationSpeed   = 1;
unsigned int volumeMultiplier = 50;

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
            if(emulationSpeed == 1){
                emulationSpeed = 2;
                setFrontendMessage("TURBO MODE ON");
            } else {
                emulationSpeed = 1;
                setFrontendMessage("TURBO MODE OFF");
            }
            break;

            case SDLK_RETURN:
                if(keyState[SDL_SCANCODE_RALT])
                    fullScreen();
            break;
        }
    }

    if(isKeyPressed){
        switch(keyPressed){
            case SDLK_F1:
            soundMute = !soundMute;
            if(soundMute)
                setFrontendMessage("AUDIO MUTED");
            else
                setFrontendMessage("AUDIO UNMUTED");
            break;

            case SDLK_F2:
            if(volumeMultiplier != 0)
                volumeMultiplier--;
            snprintf(message, 31, "VOLUME %d", volumeMultiplier);
            remaining_frames = MESSAGE_FRAME_TIME;
            break;

            case SDLK_F3:
            if(volumeMultiplier < 100)
                volumeMultiplier++;
            snprintf(message, 31, "VOLUME %d", volumeMultiplier);
            remaining_frames = MESSAGE_FRAME_TIME;
            break;

            case SDLK_F4:
            emulationStopped = !emulationStopped;
            if(emulationStopped)
                setFrontendMessage("PAUSED");
            else
                setFrontendMessage("UNPAUSED");
            break;

            case SDLK_F5:
            reset();
            setFrontendMessage("RESET");
            break;

            case SDLK_F6:
            saveState();
            setFrontendMessage("STATE SAVED");
            break;

            case SDLK_F7:
            loadState();
            setFrontendMessage("STATE LOADED");
            break;

            case SDLK_F8:
            if(usingShader){
                setScaleMode(NEAREST);
                noGlobalShader();
                setFrontendMessage("NO SHADER");
            } else {
                setScaleMode(LINEAR);
                setGlobalShader(crtShader);
                setFrontendMessage("CRT SHADER");
            }
            usingShader = !usingShader;
            break;

            case SDLK_F9:
            currentRom = (currentRom + 1 ) % romsetArrayLength;
            setFrontendMessage(romsetArray[currentRom]->name);
            reset();
            break;
        }
    }
}

void showText(const char* string, int offX, int offY){
    int tile[64];
    for(int i = 0; string[i] != '\0'; i++){
        char c = string[i];
        if(c == ' ')
            c = 31;
        // convert to maiusc ASCII
        if(c >= 'a' && c <= 'z')
            c &= ~(1 << 5);
        getTileFromROM(c, 1, tile);
        drawTile(offX+i, offY, tile);   
    }
}

void updateFrontendMessage(){
    if(!strlen(message) || !remaining_frames)
        return;
    
    int offX = VIDEO_COLS - strlen(message); 
    showText(message, offX, 1);
    remaining_frames--;
}

void setFrontendMessage(const char* string){
    strncpy(message, string, 32);
    remaining_frames = MESSAGE_FRAME_TIME;
}