#include <SDL_MAINLOOP.h>
#include <hardware.h>

void setup(){
    size(224, 288);
    setTitle("PAC-MAN");
    frameRate(60);
    initAll();
    onExit = freeAll;
}

void loop(){
    for(size_t ticks = 0; ticks < HERTZ / 60; ticks++){
        if(cpuCycles == 0){
            stepCPU();
        }

        if(audioCycles == 0){
            updateAudio();
            audioCycles = 32;
        }

        cpuCycles--;
        audioCycles--;
    }

    if(VBLANK_ENABLED){
        sendInterrupt();
    }

    drawVideo();
    updateInput();
}