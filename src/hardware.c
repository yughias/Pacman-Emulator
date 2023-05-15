#include <hardware.h>

void loadROM(const char* filename, size_t size, uint8_t* buffer){
    FILE* fptr = fopen(filename, "rb");
    fread(buffer, 1, size, fptr);
    fclose(fptr);
}

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