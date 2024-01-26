#include <hardware.h>

z80_t cpu = {
    .readMemory  = getReadMemoryAddr,
    .writeMemory = getWriteMemoryAddr,
    .readIO      = getReadIOAddr,
    .writeIO     = getWriteIOAddr 
};

void initAll(){
    initMemory();
    initVideoData();
    initAudioData();
    initCPU(&cpu);
}

void freeAll(){
    freeMemory();
    freeVideoData();
    freeAudioData();
}

void sendInterrupt(){
    cpu.INTERRUPT_PENDING = true;
    cpu.INTERRUPT_VECT = IO[0];
}

void emulateHardware(){
    for(size_t ticks = 0; ticks < HERTZ / 60; ticks++){
        if(AUX_INSTALLED && (AUX_BOARD & 0x1))
            AUX_ENABLED = true;
        if(cpu.cycles == 0)
            stepCPU(&cpu);
        cpu.cycles--;
    }

    if(VBLANK_ENABLED && cpu.INTERRUPT_ENABLED)
        sendInterrupt();
}

void reset(){
    #ifdef __EMSCRIPTEN__
        freeMemory();
        freeVideoData();
        initMemory();
        initVideoData();
        initCPU(&cpu);
    #else
        freeAll();
        initAll();
    #endif
}