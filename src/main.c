#include <SDL_MAINLOOP.h>
#include <hardware.h>

void setup(){
    size(224, 288);
    setTitle("PAC-MAN");
    setWindowIcon("data/logo.bmp");
    frameRate(60);
    initAll();
    unpauseAudio();
    onExit = freeAll;
}

void loop(){
    emulateHardware();

    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    updateInput(keyState);
    
    drawVideo();
}