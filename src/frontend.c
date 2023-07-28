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

bool         startupScreen      = true;
bool         emulationStopped = false;
bool         soundMute        = false;
unsigned int emulationSpeed   = 1;
unsigned int volumeScaler = 50;

bool         usingShader = false; 
Shader       crtShader;

void updateHotKeys(const Uint8* keyState){
    if(startupScreen){
        if(isKeyReleased && keyReleased == SDLK_RETURN)
            startupScreen = false;
        else
            return;
    }

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
            if(volumeScaler != 0)
                volumeScaler--;
            calculateVolume();
            snprintf(message, 31, "VOLUME %d", volumeScaler);
            remaining_frames = MESSAGE_FRAME_TIME;
            break;

            case SDLK_F3:
            if(volumeScaler < 100)
                volumeScaler++;
            calculateVolume();
            snprintf(message, 31, "VOLUME %d", volumeScaler);
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

void showStartupScreen(){
    background(0, 0, 0);

    showText("PACMAN CABINET EMULATOR",      0, 1);
    showText("MADE BY YUGHIAS",              0, 2);
    
    showText("SUPPORTED ROMS",               0, 5);
    showText("PACMAN         MSPACMAN",      0, 6);
    showText("PACMAN FAST    MSPACMAN FAST", 0, 7);
    showText("PACMAN HARDER  PACMAN PLUS",   0, 8);
    
    showText("CABINET CONTROLS",             0, 11);
    showText("ARROW KEYS    MOVE JOYSTICK",  0, 12);
    showText("LEFT SHIFT    INSERT COIN",    0, 13);
    showText("1             1 PLAYER START", 0, 14);
    showText("2             2 PLAYER START", 0, 15);

    showText("HOTKEYS",                      0, 18);
    showText("F1            MUTE VOLUME",    0, 19);
    showText("F2            VOLUME DOWN",    0, 20);
    showText("F3            VOLUME UP",      0, 21);
    showText("F4            PAUSE GAME",     0, 22);
    showText("F5            RESET GAME",     0, 23);
    showText("F6            SAVE STATE",     0, 24);
    showText("F7            LOAD STATE",     0, 25);
    showText("F8            RETRO SHADER",   0, 26);
    showText("F9            CHANGE ROM",     0, 27);
    showText("CAPS LOCK     TURBO MODE",     0, 28);
    showText("ALT RETURN    FULL SCREEN",    0, 29);

    showText("PRESS RETURN TO CONTINUE",     0, 32);
}