#include <frontend.h>
#include <hardware.h>
#include <romset.h>

void setup(){
    size(224, 288);
    setTitle("PAC-MAN");
    setWindowIcon("data/logo.bmp");
    setScaleMode(NEAREST);
    crtShader = loadShader("data/shader/crt.glsl");
    frameRate(60);

    initAll();
    onExit = freeAll;
}

void loop(){
    if(!emulationStopped && !startupScreen)
        for(size_t i = 0; i < emulationSpeed; i++){
            emulateHardware();
        }

    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    updateHotKeys(keyState);
    updateInput(keyState);

    drawVideo();
    updateFrontendMessage();

    if(startupScreen)
        showStartupScreen();
}