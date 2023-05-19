#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "z80.h"
#include "memory.h"
#include "video.h"
#include "input.h"
#include "audio.h"

void initAll();
void freeAll();
void emulateHardware();
void loadROM(const char*, size_t, uint8_t*);
void sendInterrupt();

#endif