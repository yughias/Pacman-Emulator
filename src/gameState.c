#include <stdlib.h>
#include "hardware.h"
#include "romset.h"

void saveState(){
    char filename[128];
    snprintf(filename, 127, "data/savestate/%s.state", romsetArray[currentRom]->name);
    FILE* fptr = fopen(filename, "wb");
    
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
    fwrite(&AUX_ENABLED,         1, 1,        fptr);
    
    // CPU dump
    fwrite(&cpu.HALTED,              1, 1,        fptr);
    fwrite(&cpu.INTERRUPT_ENABLED,   1, 1,        fptr);
    fwrite(&cpu.INTERRUPT_PENDING,   1, 1,        fptr);
    fwrite(&cpu.INTERRUPT_VECT,      2, 1,        fptr);
    fwrite(cpu.AF,                   2, 1,        fptr);
    fwrite(cpu.BC,                   2, 1,        fptr);
    fwrite(cpu.DE,                   2, 1,        fptr);
    fwrite(cpu.HL,                   2, 1,        fptr);
    fwrite(cpu.AF_,                  2, 1,        fptr);
    fwrite(cpu.BC_,                  2, 1,        fptr);
    fwrite(cpu.DE_,                  2, 1,        fptr);
    fwrite(cpu.HL_,                  2, 1,        fptr);
    fwrite(cpu.IX,                   2, 1,        fptr);
    fwrite(cpu.IY,                   2, 1,        fptr);
    fwrite(cpu.I,                    1, 1,        fptr);
    fwrite(cpu.R,                    1, 1,        fptr);
    fwrite(cpu.SP,                   2, 1,        fptr);
    fwrite(cpu.PC,                   2, 1,        fptr);
    fwrite(&cpu.cycles,              8, 1,        fptr);

    fclose(fptr);
}

void loadState(){
    char filename[128];
    snprintf(filename, 127, "data/savestate/%s.state", romsetArray[currentRom]->name);
    FILE* fptr = fopen(filename, "rb");
    
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
    fread(&AUX_ENABLED,         1, 1,        fptr);

    // CPU dump
    fread(&cpu.HALTED,              1, 1,        fptr);
    fread(&cpu.INTERRUPT_ENABLED,   1, 1,        fptr);
    fread(&cpu.INTERRUPT_PENDING,   1, 1,        fptr);
    fread(&cpu.INTERRUPT_VECT,      2, 1,        fptr);
    fread(cpu.AF,                   2, 1,        fptr);
    fread(cpu.BC,                   2, 1,        fptr);
    fread(cpu.DE,                   2, 1,        fptr);
    fread(cpu.HL,                   2, 1,        fptr);
    fread(cpu.AF_,                  2, 1,        fptr);
    fread(cpu.BC_,                  2, 1,        fptr);
    fread(cpu.DE_,                  2, 1,        fptr);
    fread(cpu.HL_,                  2, 1,        fptr);
    fread(cpu.IX,                   2, 1,        fptr);
    fread(cpu.IY,                   2, 1,        fptr);
    fread(cpu.I,                    1, 1,        fptr);
    fread(cpu.R,                    1, 1,        fptr);
    fread(cpu.SP,                   2, 1,        fptr);
    fread(cpu.PC,                   2, 1,        fptr);
    fread(&cpu.cycles,              8, 1,        fptr);

    fclose(fptr);
}