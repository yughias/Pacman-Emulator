#include <stdlib.h>
#include "hardware.h"


void saveState(){
    FILE* fptr = fopen("data/savestate.bin", "wb");
    
    // MEMORY DUMP
    fwrite(IO,                   1, IO_SIZE,  fptr);
    fwrite(RAM,                  1, RAM_SIZE, fptr);
    fwrite(&IN0,                 1, 1,        fptr);
    fwrite(&VBLANK_ENABLED,      1, 1,        fptr);
    fwrite(&SOUND_ENABLED,       1, 1,        fptr);
    fwrite(&AUX_BOARD,           1, 1,        fptr);
    fwrite(&FLIP_SCREEN,         1, 1,        fptr);
    fwrite(&P1_LAMP,             1, 1,        fptr);
    fwrite(&P2_LAMP,             1, 1,        fptr);
    fwrite(&COIN_LOCKOUT,        1, 1,        fptr);
    fwrite(&COIN_COUNTER,        1, 1,        fptr);
    fwrite(&IN1,                 1, 1,        fptr);
    fwrite(SOUND_VOICE1,         1, 6,        fptr);
    fwrite(SOUND_VOICE2,         1, 5,        fptr);
    fwrite(SOUND_VOICE3,         1, 5,        fptr);
    fwrite(VOICE1_FREQ,          1, 5,        fptr);
    fwrite(&VOICE1_VOLUME,       1, 1,        fptr);
    fwrite(VOICE2_FREQ_VOL,      1, 5,        fptr);
    fwrite(VOICE3_FREQ_VOL,      1, 5,        fptr);
    fwrite(SPRITE_COORDS,        1, 16,       fptr);
    fwrite(&DIP_SWITCH_SETTINGS, 1, 1,        fptr);
    fwrite(&WATCHDOG_RESET,      1, 1,        fptr);
    
    // CPU dump
    fwrite(&HALTED,              1, 1,        fptr);
    fwrite(&INTERRUPT_ENABLED,   1, 1,        fptr);
    fwrite(&INTERRUPT_PENDING,   1, 1,        fptr);
    fwrite(&INTERRUPT_VECT,      2, 1,        fptr);
    fwrite(AF,                   2, 1,        fptr);
    fwrite(BC,                   2, 1,        fptr);
    fwrite(DE,                   2, 1,        fptr);
    fwrite(HL,                   2, 1,        fptr);
    fwrite(AF_,                  2, 1,        fptr);
    fwrite(BC_,                  2, 1,        fptr);
    fwrite(DE_,                  2, 1,        fptr);
    fwrite(HL_,                  2, 1,        fptr);
    fwrite(IX,                   2, 1,        fptr);
    fwrite(IY,                   2, 1,        fptr);
    fwrite(I,                    1, 1,        fptr);
    fwrite(R,                    1, 1,        fptr);
    fwrite(SP,                   2, 1,        fptr);
    fwrite(PC,                   2, 1,        fptr);
    fwrite(&cpuCycles,           8, 1,        fptr);

    fclose(fptr);
}

void loadState(){
    FILE* fptr = fopen("data/savestate.bin", "rb");
    
    // MEMORY DUMP
    fread(IO,                   1, IO_SIZE,  fptr);
    fread(RAM,                  1, RAM_SIZE, fptr);
    fread(&IN0,                 1, 1,        fptr);
    fread(&VBLANK_ENABLED,      1, 1,        fptr);
    fread(&SOUND_ENABLED,       1, 1,        fptr);
    fread(&AUX_BOARD,           1, 1,        fptr);
    fread(&FLIP_SCREEN,         1, 1,        fptr);
    fread(&P1_LAMP,             1, 1,        fptr);
    fread(&P2_LAMP,             1, 1,        fptr);
    fread(&COIN_LOCKOUT,        1, 1,        fptr);
    fread(&COIN_COUNTER,        1, 1,        fptr);
    fread(&IN1,                 1, 1,        fptr);
    fread(SOUND_VOICE1,         1, 6,        fptr);
    fread(SOUND_VOICE2,         1, 5,        fptr);
    fread(SOUND_VOICE3,         1, 5,        fptr);
    fread(VOICE1_FREQ,          1, 5,        fptr);
    fread(&VOICE1_VOLUME,       1, 1,        fptr);
    fread(VOICE2_FREQ_VOL,      1, 5,        fptr);
    fread(VOICE3_FREQ_VOL,      1, 5,        fptr);
    fread(SPRITE_COORDS,        1, 16,       fptr);
    fread(&DIP_SWITCH_SETTINGS, 1, 1,        fptr);
    fread(&WATCHDOG_RESET,      1, 1,        fptr);

    // CPU dump
    fread(&HALTED,              1, 1,        fptr);
    fread(&INTERRUPT_ENABLED,   1, 1,        fptr);
    fread(&INTERRUPT_PENDING,   1, 1,        fptr);
    fread(&INTERRUPT_VECT,      2, 1,        fptr);
    fread(AF,                   2, 1,        fptr);
    fread(BC,                   2, 1,        fptr);
    fread(DE,                   2, 1,        fptr);
    fread(HL,                   2, 1,        fptr);
    fread(AF_,                  2, 1,        fptr);
    fread(BC_,                  2, 1,        fptr);
    fread(DE_,                  2, 1,        fptr);
    fread(HL_,                  2, 1,        fptr);
    fread(IX,                   2, 1,        fptr);
    fread(IY,                   2, 1,        fptr);
    fread(I,                    1, 1,        fptr);
    fread(R,                    1, 1,        fptr);
    fread(SP,                   2, 1,        fptr);
    fread(PC,                   2, 1,        fptr);
    fread(&cpuCycles,           8, 1,        fptr);

    fclose(fptr);
}