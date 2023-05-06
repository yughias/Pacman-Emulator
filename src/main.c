#include <SDL_MAINLOOP.h>
#include <hardware.h>

void setup(){
    size(224, 288);
    frameRate(60);
    initAll();
    onExit = freeAll;
}

void loop(){
    for(size_t ticks = 0; ticks < HERTZ / 60; ticks++){
        if(cpuCycles != 0){
            stepCPU();
            cpuCycles = 4;
        }
        cpuCycles--;
    }

    sendInterrupt();
    updateInput();
}