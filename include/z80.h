#ifndef _Z80_H_
#define _Z80_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct z80_t z80_t;
typedef uint8_t* (*busFunc)(uint16_t);

typedef struct z80_t {
    // interrupt vars
    bool     HALTED;
    bool     INTERRUPT_ENABLED;
    bool     INTERRUPT_PENDING;
    uint8_t  INTERRUPT_MODE;
    uint8_t INTERRUPT_VECT;

    // 16 bit regs 
    uint16_t AF[1];
    uint16_t BC[1];
    uint16_t DE[1];
    uint16_t HL[1];
    uint16_t AF_[1];
    uint16_t BC_[1];
    uint16_t DE_[1];
    uint16_t HL_[1];
    uint16_t IX[1];
    uint16_t IY[1];
    uint16_t SP[1];
    uint16_t PC[1];

    // 8 bit regs
    uint8_t* A;
    uint8_t* F;
    uint8_t* B;
    uint8_t* C;
    uint8_t* D;
    uint8_t* E;
    uint8_t* H;
    uint8_t* L;
    uint8_t* IXL;
    uint8_t* IXH;
    uint8_t* IYL;
    uint8_t* IYH;

    uint8_t I[1];
    uint8_t R[1];

    // busFunc
    busFunc readMemory;
    busFunc writeMemory;

    busFunc readIO;
    busFunc writeIO;

    uint64_t cycles;
} z80_t;


void initCPU(z80_t*);
void infoCPU(z80_t*);
void stepCPU(z80_t*);

#endif