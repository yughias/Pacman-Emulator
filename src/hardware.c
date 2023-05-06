#include <hardware.h>

void loadROM(const char* filename, size_t size, uint8_t* buffer){
    FILE* fptr = fopen(filename, "rb");
    fread(buffer, 1, size, fptr);
    fclose(fptr);
}

void initAll(){
    initMemory();
    initVideoData();
    initCPU();
}

void freeAll(){
    freeMemory();
    freeVideoData();
}

void sendInterrupt(){
    if(VBLANK_ENABLED){
        INTERRUPT_PENDING = true;
        INTERRUPT_VECT = IO[0];
    }
    drawVideo();
}