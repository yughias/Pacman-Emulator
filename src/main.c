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
            cpuCycles = 1;
            stepCPU();
        }
        cpuCycles--;
    }

    sendInterrupt();
    updateInput();
}