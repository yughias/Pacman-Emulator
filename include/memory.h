#ifndef _MEMORY_H_
#define _MEMORY_H_

#define IO_SIZE        0x0001
#define ROM_SIZE       0x4000
#define RAM_SIZE       0x1000
#define TILE_RAM       0x4000
#define PALETTE_RAM    0x4400
#define SPRITE_RAM     0x4FF0

// I/0 SPACE
extern uint8_t IO[IO_SIZE];

// MEMORY
extern uint8_t* ROM;
extern uint8_t* RAM;

// AUX BORD
extern uint8_t* AUX_ROM_LOW;
extern uint8_t* AUX_ROM_HIGH;
extern bool AUX_INSTALLED;
extern bool AUX_ENABLED;

// Memory Mapped Registers
extern uint8_t IN0;
extern uint8_t VBLANK_ENABLED;
extern uint8_t SOUND_ENABLED;
extern uint8_t AUX_BOARD;
extern uint8_t FLIP_SCREEN;
extern uint8_t P1_LAMP;
extern uint8_t P2_LAMP;
extern uint8_t COIN_LOCKOUT;
extern uint8_t COIN_COUNTER;
extern uint8_t IN1;
extern uint8_t SOUND_VOICE1[6];
extern uint8_t SOUND_VOICE2[5];
extern uint8_t SOUND_VOICE3[5];
extern uint8_t VOICE1_FREQ[5];
extern uint8_t VOICE1_VOLUME;
extern uint8_t VOICE2_FREQ_VOL[5];
extern uint8_t VOICE3_FREQ_VOL[5];
extern uint8_t SPRITE_COORDS[16];
extern uint8_t DIP_SWITCH_SETTINGS;
extern uint8_t WATCHDOG_RESET;
extern uint8_t NOT_MAPPED[2];


void initMemory();
void freeMemory();
uint8_t* getReadMemoryAddr(uint16_t);
uint8_t* getWriteMemoryAddr(uint16_t);
uint8_t* getReadIOAddr(uint16_t);
uint8_t* getWriteIOAddr(uint16_t);


#endif