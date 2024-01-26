#ifndef _ROMSET_H__
#define _ROMSET_H__

#include <stdint.h>
#include <stddef.h>

typedef void (*initFunc)();

typedef struct {
    const char name[32];
    initFunc initMemory;
    initFunc initVideo;
} RomSet;

extern RomSet pacman;
extern RomSet mspacman;

extern RomSet* romsetArray[];
extern int romsetArrayLength; 
extern int currentRom;
extern char romPath[];

void loadROM(const char*, size_t, uint8_t*);

// init functions
void initMemory_pacman();
void initMemory_mspacman();
void initMemory_pacmanf();
void initMemory_mspacmnf();
void initMemory_pacmod();
void initMemory_pacplus();

void initVideo_pacman();
void initVideo_mspacman();
void initVideo_pacmod();
void initVideo_pacplus();

#endif