#include <hardware.h>
#include <romset.h>
#include <SDL_MAINLOOP.h>

// I/0 SPACE
uint8_t IO[IO_SIZE];

// MEMORY
uint8_t* ROM;
uint8_t* RAM;

// AUX BOARD
uint8_t* AUX_ROM_LOW;
uint8_t* AUX_ROM_HIGH;
bool AUX_INSTALLED;
bool AUX_ENABLED;

// Memory Mapped Registers
uint8_t IN0;
uint8_t VBLANK_ENABLED;
uint8_t SOUND_ENABLED;
uint8_t AUX_BOARD;
uint8_t FLIP_SCREEN;
uint8_t P1_LAMP;
uint8_t P2_LAMP;
uint8_t COIN_LOCKOUT;
uint8_t COIN_COUNTER;
uint8_t IN1;
uint8_t SOUND_VOICE1[6];
uint8_t SOUND_VOICE2[5];
uint8_t SOUND_VOICE3[5];
uint8_t VOICE1_FREQ[5];
uint8_t VOICE1_VOLUME;
uint8_t VOICE2_FREQ_VOL[5];
uint8_t VOICE3_FREQ_VOL[5];
uint8_t SPRITE_COORDS[16];
uint8_t DIP_SWITCH_SETTINGS;
uint8_t WATCHDOG_RESET;
uint8_t NOT_MAPPED[2];

void initMemory(){
    romsetArray[currentRom]->initMemory();

    AUX_ENABLED           = false;
    IN0                   = 0xFF;
    VBLANK_ENABLED        = 0x00;
    SOUND_ENABLED         = 0x00;
    AUX_BOARD             = 0x00;
    FLIP_SCREEN           = 0x00;
    P1_LAMP               = 0x00;
    P2_LAMP               = 0x00;
    COIN_LOCKOUT          = 0x00;
    COIN_COUNTER          = 0xFF;
    IN1                   = 0xFF;
    memset(SOUND_VOICE1,    0x00, 6);
    memset(SOUND_VOICE2,    0x00, 5);
    memset(SOUND_VOICE3,    0x00, 5);
    memset(VOICE1_FREQ,     0x00, 5);
    VOICE1_VOLUME         = 0x00;
    memset(VOICE2_FREQ_VOL, 0x00, 5);
    memset(VOICE3_FREQ_VOL, 0x00, 5);
    memset(SPRITE_COORDS,   0x00, 16);
    DIP_SWITCH_SETTINGS   = 0xF9;
    WATCHDOG_RESET        = 0x00;
}

void freeMemory(){
    free(ROM);
    free(RAM);
    free(AUX_ROM_HIGH);
    free(AUX_ROM_LOW);
}

uint8_t* getReadAddress(uint16_t address){
    if(AUX_INSTALLED && AUX_ENABLED){
        if(address < ROM_SIZE)
            return AUX_ROM_LOW + address;
        if(address >= 0x8000 && address < 0x8800) // U5
            return AUX_ROM_HIGH + (address - 0x8000);
        if(address >= 0x8800){ // U6
            return AUX_ROM_HIGH + ((address & 0xFFF) + 0x800);
        }
    }

    if(address < ROM_SIZE)
        return ROM + address;
    else if(address < 0x5000)
        return RAM + (address - ROM_SIZE);
    else if(address == 0x5003)
        return &FLIP_SCREEN;
    else if(address == 0x5004)
        return &P1_LAMP;
    else if(address == 0x5005)
        return &P2_LAMP;
    else if(address == 0x5006)
        return &COIN_LOCKOUT;
    else if(address == 0x5007)
        return &COIN_COUNTER;
    else if(address < 0x5040)
        return &IN0;
    else if(address < 0x5080)
        return &IN1;
    else
        return &DIP_SWITCH_SETTINGS;
}

uint8_t* getWriteAddress(uint16_t address){
    address &= 0x7fff;

    if(address < ROM_SIZE){
        printf("WRITE ON ROM ERROR! %X\n", address);
    }

    if(address < ROM_SIZE)
        return ROM + address;
    else if(address < 0x5000)
        return RAM + (address - ROM_SIZE);
    else if(address == 0x5000)
        return &VBLANK_ENABLED;
    else if(address == 0x5001)
        return &SOUND_ENABLED;
    else if(address == 0x5002)
        return &AUX_BOARD;
    else if(address == 0x5003)
        return &FLIP_SCREEN;
    else if(address == 0x5004)
        return &P1_LAMP;
    else if(address == 0x5005)
        return &P2_LAMP;
    else if(address == 0x5006)
        return &COIN_LOCKOUT;
    else if(address == 0x5007)
        return &COIN_COUNTER;
    else if(address >= 0x5040 && address <= 0x5045)
        return SOUND_VOICE1 + (address - 0x5040);
    else if(address >= 0x5046 && address <= 0x504A)
        return SOUND_VOICE2 + (address - 0x5046);
    else if(address >= 0x504B && address <= 0x504F)
        return SOUND_VOICE3 + (address - 0x504B);
    else if(address >= 0x5050 && address <= 0x5054)
        return VOICE1_FREQ + (address - 0x5050);
    else if(address == 0x5055)
        return &VOICE1_VOLUME;
    else if(address >= 0x5056 && address <= 0x505A)
        return VOICE2_FREQ_VOL + (address - 0x5056);
    else if(address >= 0x505B && address <= 0x505F)
        return VOICE3_FREQ_VOL + (address - 0x505B);
    else if(address >= 0x5060 && address <= 0x506F)
        return SPRITE_COORDS + (address - 0x5060);
    else if(address >= 0x50C0 && address <= 0x50FF)
        return &WATCHDOG_RESET;
    else
        return NOT_MAPPED;
}