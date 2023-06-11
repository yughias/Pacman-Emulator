#include <SDL_MAINLOOP.h>
#include <frontend.h>
#include <hardware.h>

void setup(){
    size(224, 288);
    setTitle("PAC-MAN");
    setWindowIcon("data/logo.bmp");
    frameRate(60);
    initAll();
    printInfo();
    unpauseAudio();
    onExit = freeAll;
}

void loop(){
    if(!emulationStopped)
        for(size_t i = 0; i < emulationSpeed; i++)
            emulateHardware();

    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    updateInput(keyState);
    updateHotKeys(keyState);

    drawVideo();
}