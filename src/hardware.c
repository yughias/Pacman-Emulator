#include <hardware.h>

void initAll(){
    initMemory();
    initVideoData();
    initAudioData();
    initCPU();
}

void freeAll(){
    freeMemory();
    freeVideoData();
    freeAudioData();
}

void sendInterrupt(){
    INTERRUPT_PENDING = true;
    INTERRUPT_VECT = IO[0];
}

void emulateHardware(){
    for(size_t ticks = 0; ticks < HERTZ / 60; ticks++){
        if(AUX_INSTALLED && (AUX_BOARD & 0x1))
            AUX_ENABLED = true;
        if(cpuCycles == 0)
            stepCPU();
        cpuCycles--;
    }

    if(VBLANK_ENABLED && INTERRUPT_ENABLED)
        sendInterrupt();
}

void reset(){
    freeAll();
    initAll();
}