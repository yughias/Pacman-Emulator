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

    #ifdef __EMSCRIPTEN__
    startupScreen = false;
    volumeScaler = 100;
    calculateVolume();
    #endif
}

void loop(){
    if(!emulationStopped && !startupScreen)
        for(size_t i = 0; i < emulationSpeed; i++){
            emulateHardware();
        }

    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    
    #ifndef __EMSCRIPTEN__
        updateHotKeys(keyState);
    #endif

    updateInput(keyState);

    drawVideo();
    updateFrontendMessage();

    if(startupScreen)
        showStartupScreen();
}