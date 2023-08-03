#include <z80.h>
#include <stdio.h>

typedef void (*rotateFunc)(z80_t*, uint8_t*);
typedef void (*aluFunc)(z80_t*, uint8_t*, uint8_t);
typedef void (*blockFunc)();

void useDDRegisterTable(z80_t*, uint8_t**, uint16_t**, uint16_t**);
void useFDRegisterTable(z80_t*, uint8_t**, uint16_t**, uint16_t**);
void restoreRegisterTable(z80_t*, uint8_t**, uint16_t**, uint16_t**);
void adjustDDorFFOpcode(z80_t*, uint8_t**, uint16_t**, uint16_t**);

//z80 NEW INSTRUCTION
void NOP(z80_t*);
void EX(z80_t*, uint16_t*, uint16_t*);
void DJNZ(z80_t*, int8_t);
void JR(z80_t*, int8_t);
void JRNZ(z80_t*, int8_t);
void JRZ(z80_t*, int8_t);
void JRNC(z80_t*, int8_t);
void JRC(z80_t*, int8_t);
void LD_8(z80_t*, uint8_t*, uint8_t);
void LD_16(z80_t*, uint16_t*, uint16_t);
void ADD_16(z80_t*, uint16_t*, uint16_t*);
void INC_16(z80_t*, uint16_t*);
void DEC_16(z80_t*, uint16_t*);
void INC_8(z80_t*, uint8_t*);
void DEC_8(z80_t*, uint8_t*);
void DAA(z80_t*);
void CPL(z80_t*);
void SCF(z80_t*);
void CCF(z80_t*);
void HLT(z80_t*);
void RETNZ(z80_t*);
void RETZ(z80_t*);
void RETNC(z80_t*);
void RETC(z80_t*);
void RETPO(z80_t*);
void RETPE(z80_t*);
void RETP(z80_t*);
void RETM(z80_t*);
void POP(z80_t*, uint16_t*);
void RET(z80_t*);
void EXX(z80_t*);
void JP(z80_t*, uint16_t);
void JPNZ(z80_t*, uint16_t);
void JPZ(z80_t*, uint16_t);
void JPNC(z80_t*, uint16_t);
void JPC(z80_t*, uint16_t);
void JPPO(z80_t*, uint16_t);
void JPPE(z80_t*, uint16_t);
void JPP(z80_t*, uint16_t);
void JPM(z80_t*, uint16_t);
void OPCODE_OUT(z80_t*, uint8_t, uint16_t);
void OPCODE_IN(z80_t*, uint8_t*, uint16_t);
void EI(z80_t*);
void DI(z80_t*);
void CALL(z80_t*, uint16_t);
void CALLNZ(z80_t*, uint16_t);
void CALLZ(z80_t*, uint16_t);
void CALLNC(z80_t*, uint16_t);
void CALLC(z80_t*, uint16_t);
void CALLPO(z80_t*, uint16_t);
void CALLPE(z80_t*, uint16_t);
void CALLP(z80_t*, uint16_t);
void CALLM(z80_t*, uint16_t);
void PUSH(z80_t*, uint16_t);
void RST(z80_t*, uint8_t);
void RLC(z80_t*, uint8_t*);
void RRC(z80_t*, uint8_t*);
void RLCA(z80_t*);
void RRCA(z80_t*);
void RLA(z80_t*);
void RRA(z80_t*);
void RLC(z80_t*, uint8_t*);
void RRC(z80_t*, uint8_t*);
void RL(z80_t*, uint8_t*);
void RR(z80_t*, uint8_t*);
void SLA(z80_t*, uint8_t*);
void SRA(z80_t*, uint8_t*);
void SLL(z80_t*, uint8_t*);
void SRL(z80_t*, uint8_t*);
void BIT(z80_t*, uint8_t, uint8_t*);
void SET(z80_t*, uint8_t, uint8_t*);
void RES(z80_t*, uint8_t, uint8_t*);
void ADD(z80_t*, uint8_t*, uint8_t);
void ADC(z80_t*, uint8_t*, uint8_t);
void SUB(z80_t*, uint8_t*, uint8_t);
void SBC(z80_t*, uint8_t*, uint8_t);
void AND(z80_t*, uint8_t*, uint8_t);
void XOR(z80_t*, uint8_t*, uint8_t);
void OR(z80_t*, uint8_t*, uint8_t);
void CP(z80_t*, uint8_t*, uint8_t);
void ADC_16(z80_t*, uint16_t*, uint16_t);
void SBC_16(z80_t*, uint16_t*, uint16_t);
void NEG(z80_t*, uint8_t*);
void RETI(z80_t*);
void RETN(z80_t*);
void IM(z80_t*, uint8_t);
void RRD(z80_t*);
void RLD(z80_t*);

// block instruction
void LDI(z80_t*);
void LDD(z80_t*);
void LDIR(z80_t*);
void LDDR(z80_t*);
void CPI(z80_t*);
void CPD(z80_t*);
void CPIR(z80_t*);
void CPDR(z80_t*);
void INI(z80_t*);
void IND(z80_t*);
void INIR(z80_t*);
void INDR(z80_t*);
void OUTI(z80_t*);
void OUTD(z80_t*);
void OTIR(z80_t*);
void OTDR(z80_t*);

// flag masks to set/clear registers
#define SET_S   0b10000000
#define SET_Z   0b01000000
#define SET_Y   0b00100000
#define SET_H   0b00010000
#define SET_X   0b00001000
#define SET_P   0b00000100
#define SET_N   0b00000010
#define SET_C   0b00000001

#define CLEAR_S 0b01111111
#define CLEAR_Z 0b10111111
#define CLEAR_Y 0b11011111
#define CLEAR_H 0b11101111
#define CLEAR_X 0b11110111
#define CLEAR_P 0b11111011
#define CLEAR_N 0b11111101
#define CLEAR_C 0b11111110

#define SET_FLAG(f)          *z80->F |= SET_ ## f
#define CLEAR_FLAG(f)        *z80->F &= CLEAR_ ## f
#define CHANGE_FLAG(f, val)  *z80->F ^= (-!!(val) ^ *z80->F) & SET_ ## f

// cpu utility function
void setParity(z80_t*, uint8_t);
void setZero(z80_t*, uint16_t);
void setSign8Bit(z80_t*, uint8_t);
void setSign16Bit(z80_t*, uint16_t);
bool calculateCarry(int, uint16_t, uint16_t, bool);

void initCPU(z80_t* z80){
    z80->cycles += 0;
    z80->F = (uint8_t*)z80->AF;
    z80->A = z80->F + 1;
    z80->C = (uint8_t*)z80->BC;
    z80->B = z80->C + 1;
    z80->E = (uint8_t*)z80->DE;
    z80->D = z80->E + 1;
    z80->L = (uint8_t*)z80->HL;
    z80->H = z80->L + 1;
    z80->IXL = (uint8_t*)z80->IX;
    z80->IXH = z80->IXL + 1;
    z80->IYL = (uint8_t*)z80->IY;
    z80->IYH = z80->IYL + 1;
 
    *z80->AF = 0xFFFF;
    *z80->BC = 0;
    *z80->DE = 0;
    *z80->HL = 0;
    *z80->SP = 0xFFFF;
    *z80->PC = 0;

    *z80->AF_ = 0;
    *z80->BC_ = 0;
    *z80->DE_ = 0;
    *z80->HL_ = 0;
    *z80->I = 0;
    *z80->R = 0;
    z80->HALTED = false;
    z80->INTERRUPT_MODE    = 1;
    z80->INTERRUPT_ENABLED = false;
    z80->INTERRUPT_PENDING = false;
    z80->INTERRUPT_VECT = 0x0000;
    z80->cycles = 0;
}

void restoreRegisterTable(z80_t* z80, uint8_t** r, uint16_t** rp, uint16_t** rp2){
    r[0] = z80->B;
    r[1] = z80->C;
    r[2] = z80->D;
    r[3] = z80->E;
    r[4] = z80->H;
    r[5] = z80->L;
    r[6] = (uint8_t*)(uintptr_t)*z80->HL;
    r[7] = z80->A;
    rp[0] = z80->BC;
    rp[1] = z80->DE;
    rp[2] = z80->HL;
    rp[3] = z80->SP;
    rp2[0] = z80->BC;
    rp2[1] = z80->DE;
    rp2[2] = z80->HL;
    rp2[3] = z80->AF;
}

void useDDRegisterTable(z80_t* z80, uint8_t** r, uint16_t** rp, uint16_t** rp2){
    r[4] = z80->IXH;
    r[5] = z80->IXL;
    r[6] = (uint8_t*)(uintptr_t)( *z80->IX + (int8_t)*z80->readMemory(*z80->PC+2) );
    rp[2] = z80->IX;
    rp2[2] = z80->IX;
}

void useFDRegisterTable(z80_t* z80, uint8_t** r, uint16_t** rp, uint16_t** rp2){
    r[4] = z80->IYH;
    r[5] = z80->IYL;
    r[6] = (uint8_t*)(uintptr_t)( *z80->IY + (int8_t)*z80->readMemory(*z80->PC+2) );
    rp[2] = z80->IY;
    rp2[2] = z80->IY;
}

void adjustDDorFFOpcode(z80_t* z80, uint8_t** r, uint16_t** rp, uint16_t** rp2){
    *z80->PC += 1;
    r[4] = z80->H;
    r[5] = z80->L;
}

void infoCPU(z80_t* z80){
    fprintf(stderr, "PC: %04X, AF: %04X, BC: %04X, DE: %04X, HL: %04X, SP: %04X, "
         "IX: %04X, IY: %04X\n",
      *z80->PC, *z80->AF, *z80->BC, *z80->DE, *z80->HL, *z80->SP, *z80->IX, *z80->IY);

    fprintf(stderr, "S: %d ", (bool)(*z80->F & SET_S));
    fprintf(stderr, "Z: %d ", (bool)(*z80->F & SET_Z));
    fprintf(stderr, "C: %d ", (bool)(*z80->F & SET_C));
    fprintf(stderr, "P: %d ", (bool)(*z80->F & SET_P));
    fprintf(stderr, "A: %d\n", (bool)(*z80->F & SET_H));
    fprintf(stderr, "OPCODE: 0x%02X %02X %02X\n", *z80->readMemory(*z80->PC), *z80->readMemory(*z80->PC+1), *z80->readMemory(*z80->PC+2));
    fprintf(stderr, "cycles: %llu\n\n", z80->cycles);

    /*
    fprintf(stderr, "%llu ", z80->cycles);
    fprintf(stderr, "$%04X\t", *z80->PC);
    fprintf(stderr, "AF :0x%04X BC: 0x%04X ", *z80->AF, *z80->BC);
    fprintf(stderr, "DE: 0x%04X HL: 0x%04X\n", *z80->DE, *z80->HL);
    fprintf(stderr, "AF_ :0x%04X BC_: 0x%04X DE_: 0x%04X HL_: 0x%04X\n", *z80->AF_, *z80->BC_, *z80->DE_, *z80->HL_);
    fprintf(stderr, "IX: 0x%04X IY: 0x%04X\n", *z80->IX, *z80->IY);
    fprintf(stderr, "S: %d ", (bool)(*z80->F & SET_S));
    fprintf(stderr, "Z: %d ", (bool)(*z80->F & SET_Z));
    fprintf(stderr, "C: %d ", (bool)(*z80->F & SET_C));
    fprintf(stderr, "P: %d ", (bool)(*z80->F & SET_P));
    fprintf(stderr, "A: %d ", (bool)(*z80->F & SET_H));
    fprintf(stderr, "SP: 0x%04X\n", *z80->SP);
    fprintf(stderr, "Stack: 0x%04X\n", *(uint16_t*)z80->readMemory(*z80->SP));
    fprintf(stderr, "OPCODE: 0x%02X %02X %02X", *z80->readMemory(*z80->PC), *z80->readMemory(*z80->PC+1), *z80->readMemory(*z80->PC+2));
    fprintf(stderr, "\n\n");
    */
}

void processInterrupt(z80_t* z80){
    if(z80->INTERRUPT_ENABLED && z80->INTERRUPT_PENDING){
            z80->INTERRUPT_ENABLED = false;
            z80->INTERRUPT_PENDING = false;
            z80->HALTED = false;
        switch(z80->INTERRUPT_MODE){
            case 1:
            RST(z80, 0x07);
            z80->cycles += 13;
            break;
            
            case 2:
            uint16_t interruptAddress = *(uint16_t*)z80->readMemory((*z80->I << 8) | z80->INTERRUPT_VECT);
            CALL(z80, interruptAddress);
            z80->cycles += 19;
            break;
        }
    }
}

void stepCPU(z80_t* z80){
    if(z80->INTERRUPT_ENABLED && z80->INTERRUPT_PENDING){
        processInterrupt(z80);
        return;
    }

    if(z80->HALTED){
        z80->cycles += 1;
        return;
    }
        
    #ifdef DEBUG
        infoCPU(z80);
    #endif

    const static rotateFunc rot[8]  = { RLC, RRC, RL,   RR,   SLA, SRA,   SLL,  SRL   };
    const static aluFunc    alu[8]  = { ADD, ADC, SUB,  SBC,  AND,  XOR,  OR,   CP    };
    const static blockFunc  bli[16] = { LDI, LDD, LDIR, LDDR, CPI,  CPD,  CPIR, CPDR,
                                        INI, IND, INIR, INDR, OUTI, OUTD, OTIR, OTDR  };
    const static uint8_t    im[8]   = {   0,   0,    1,    2,    0,    0,    1,    2  };

    uint8_t* r[8];
    uint16_t* rp[4];
    uint16_t* rp2[4];

    uint8_t opcode = *z80->readMemory(*z80->PC);
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t q;
    uint8_t p;

    uint8_t  val8;
    uint16_t val16;
    uint16_t nn;
    uint16_t old_PC;
    bool prefixDD = false;
    bool prefixFD = false;

    restoreRegisterTable(z80, r, rp, rp2);
    switch(opcode){
        case 0xDD:
        case 0xFD:
        switch(opcode){
            case 0xDD:
            useDDRegisterTable(z80, r, rp, rp2);
            prefixDD = true;
            break;

            case 0xFD:
            useFDRegisterTable(z80, r, rp, rp2);
            prefixFD = true;
            break;
        }
        *z80->PC += 1;
        break;
    }

    opcode = *z80->readMemory(*z80->PC);

    switch(opcode){
        case 0XCB:
        if(prefixDD || prefixFD)
            *z80->PC += 1;

        *z80->PC += 1;
        opcode = *z80->readMemory(*z80->PC);
        x = opcode >> 6;
        y = (opcode >> 3) & 0b111;
        z = opcode & 0b111;
        q = y & 0b1;
        p = (y >> 1) & 0b11;

        *z80->PC += 1;
        uint8_t memptr = (uint8_t)(uintptr_t)r[6];
        if(z == 6){
            if(x == 1)
                r[z] = z80->readMemory((uintptr_t)r[z]);
            else
                r[z] = z80->writeMemory((uintptr_t)r[z]);
        }
        switch(x){
            case 0:
            rotateFunc function = rot[y];
            (*function)(z80, r[z]);
            break;

            case 1:
            BIT(z80, y, r[z]);
            // XY FLAGS SPECIAL BEHAVIOUR IF (HL) OR (IX+d) OR (IY+d) IS PARAMETER ON BIT INSTRUCTION
            if(z == 6){
                if(memptr & 0b100000)
                    *z80->F |= SET_Y;
                else 
                    *z80->F &= CLEAR_Y; 

                if(memptr & 0b1000)
                    *z80->F |= SET_X;
                else 
                    *z80->F &= CLEAR_X;
            } 
            break;

            case 2:
            RES(z80, y, r[z]);
            break;

            case 3:
            SET(z80, y, r[z]);
            break;
        }
        // BIT INSTRUCTIONS LAST A LITTLE LESS
        if(z == 6 && (prefixDD || prefixFD)){
            if(x == 1)
                z80->cycles += 20;
            else
                z80->cycles += 23;
        } else if(z == 6 && !prefixDD && !prefixDD){
            if(x == 1)
                z80->cycles += 12;
            else
                z80->cycles += 15;
        } else
            z80->cycles += 8;
        break;

        case 0XED:
        *z80->PC = *z80->PC + 1;
        opcode = *z80->readMemory(*z80->PC);
        x = opcode >> 6;
        y = (opcode >> 3) & 0b111;
        z = opcode & 0b111;
        q = y & 0b1;
        p = (y >> 1) & 0b11;

        switch(x){
            case 0:
            case 3:
            *z80->PC += 1;
            NOP(z80);
            break;

            case 1:
            switch(z){
                case 0:
                *z80->PC += 1;
                if(y != 6){
                    OPCODE_IN(z80, r[y], *z80->BC);
                    setSign8Bit(z80, *r[y]);
                    setParity(z80, *r[y]);
                    setSign8Bit(z80, *r[y]);
                } else {
                    uint8_t copy = *z80->A;
                    OPCODE_IN(z80, z80->A, *z80->BC);
                    setSign8Bit(z80, *z80->A);
                    setParity(z80, *z80->A);
                    setSign8Bit(z80, *z80->A);
                    *z80->A = copy;
                }
                z80->cycles += 12;
                break;

                case 1:
                *z80->PC += 1;
                if(y != 6){
                    OPCODE_OUT(z80, *r[y], *z80->BC);
                    setSign8Bit(z80, *r[y]);
                    setParity(z80, *r[y]);
                    setSign8Bit(z80, *r[y]);
                } else {
                    OPCODE_OUT(z80, 0, *z80->BC);
                    setSign8Bit(z80, *z80->A);
                    setParity(z80, *z80->A);
                    setSign8Bit(z80, *z80->A);
                }
                z80->cycles += 12;
                break;

                case 2:
                *z80->PC += 1;
                if(q == 0)
                    SBC_16(z80, z80->HL, *rp[p]);
                else
                    ADC_16(z80, z80->HL, *rp[p]);
                z80->cycles += 15;
                break;

                case 3:
                nn = *(uint16_t*)z80->readMemory(*z80->PC+1);
                *z80->PC += 3;
                if(q == 0)
                    LD_16(z80, (uint16_t*)z80->writeMemory(nn), *rp[p]);
                else 
                    LD_16(z80, rp[p], *(uint16_t*)z80->readMemory(nn));
                if(rp[p] == z80->HL)
                    z80->cycles += 16;
                else
                    z80->cycles += 20;
                break;

                case 4:
                *z80->PC += 1;
                NEG(z80, z80->A);
                z80->cycles += 8;
                break;

                case 5:
                *z80->PC += 1;
                if(y != 1)
                    RETN(z80);
                else
                    RETI(z80);
                z80->cycles += 14;
                break;

                case 6:
                *z80->PC += 1;
                IM(z80, im[y]);
                z80->cycles += 8;
                break;

                case 7:
                *z80->PC += 1;
                switch(y){
                    case 0:
                    LD_8(z80, z80->I, *z80->A);
                    z80->cycles += 9;
                    break;

                    case 1:
                    LD_8(z80, z80->R, *z80->A);
                    z80->cycles += 9;
                    break;

                    case 2:
                    LD_8(z80, z80->A, *z80->I);
                    z80->cycles += 9;
                    break;

                    case 3:
                    LD_8(z80, z80->A, *z80->R);
                    z80->cycles += 9;
                    break;

                    case 4:
                    RRD(z80);
                    z80->cycles += 18;
                    break;

                    case 5:
                    RLD(z80);
                    z80->cycles += 18;
                    break;

                    case 6:
                    NOP(z80);
                    break;

                    case 7:
                    NOP(z80);
                    break;
                }
                break;
            }
            break;

            case 2:
            *z80->PC += 1;
            blockFunc function = bli[(y-4)+z*4];
            (*function)(z80);
            break;
        }
        break;

        default:
        x = opcode >> 6;
        y = (opcode >> 3) & 0b111;
        z = opcode & 0b111;
        q = y & 0b1;
        p = (y >> 1) & 0b11;

        switch(x){
            case 0:
            switch(z){
                case 0:
                switch(y){
                    case 0:
                    *z80->PC += 1;
                    NOP(z80);
                    break;

                    case 1:
                    *z80->PC += 1;
                    EX(z80, z80->AF, z80->AF_);
                    z80->cycles += 4;
                    break;

                    case 2:
                    *z80->PC += 2;
                    DJNZ(z80, *z80->readMemory(*z80->PC-1));
                    break;

                    case 3:
                    *z80->PC += 2;
                    JR(z80, *z80->readMemory(*z80->PC-1));
                    z80->cycles += 12;
                    break;

                    case 4:
                    *z80->PC += 2;
                    old_PC = *z80->PC;
                    JRNZ(z80, *z80->readMemory(*z80->PC-1));
                    if(old_PC == *z80->PC)
                        z80->cycles += 7;
                    else
                        z80->cycles += 12;
                    break;

                    case 5:
                    *z80->PC += 2;
                    old_PC = *z80->PC;
                    JRZ(z80, *z80->readMemory(*z80->PC-1));
                    if(old_PC == *z80->PC)
                        z80->cycles += 7;
                    else
                        z80->cycles += 12;
                    break;

                    case 6:
                    *z80->PC += 2;
                    old_PC = *z80->PC;
                    JRNC(z80, *z80->readMemory(*z80->PC-1));
                    if(old_PC == *z80->PC)
                        z80->cycles += 7;
                    else
                        z80->cycles += 12;
                    break;

                    case 7:
                    *z80->PC += 2;
                    old_PC = *z80->PC;
                    JRC(z80, *z80->readMemory(*z80->PC-1));
                    if(old_PC == *z80->PC)
                        z80->cycles += 7;
                    else
                        z80->cycles += 12;
                    break;
                }
                break;

                case 1:
                switch(q){
                    case 0:
                    nn = *(uint16_t*)z80->readMemory(*z80->PC+1);
                    *z80->PC += 3;
                    LD_16(z80, rp[p], nn);
                    if(rp[p] == z80->IX || rp[p] == z80->IY)
                        z80->cycles += 14;
                    else
                        z80->cycles += 10;
                    break;

                    case 1:
                    *z80->PC += 1;
                    ADD_16(z80, rp[2], rp[p]);
                    if(rp[2] == z80->HL)
                        z80->cycles += 11;
                    else 
                        z80->cycles += 15;
                    break;
                }
                break;

                case 2:
                switch(q){
                    case 0:
                    switch(p){
                        case 0:
                        *z80->PC += 1;
                        LD_8(z80, z80->writeMemory(*z80->BC), *z80->A);
                        z80->cycles += 7;
                        break;

                        case 1:
                        *z80->PC += 1;
                        LD_8(z80, z80->writeMemory(*z80->DE), *z80->A);
                        z80->cycles += 7;
                        break;

                        case 2:
                        nn = *(uint16_t*)z80->readMemory(*z80->PC+1);
                        *z80->PC += 3;
                        LD_16(z80, (uint16_t*)z80->writeMemory(nn), *rp[2]);
                        if(rp[2] == z80->HL)
                            z80->cycles += 16;
                        else
                            z80->cycles += 20;
                        break;

                        case 3:
                        nn = *(uint16_t*)z80->readMemory(*z80->PC+1);
                        *z80->PC += 3;
                        LD_8(z80, (uint8_t*)z80->writeMemory(nn), *z80->A);
                        z80->cycles += 13;
                        break;
                    }
                    break;

                    case 1:
                    switch(p){
                        case 0:
                        *z80->PC += 1;
                        LD_8(z80, z80->A, *z80->readMemory(*z80->BC));
                        z80->cycles += 7;
                        break;

                        case 1:
                        *z80->PC += 1;
                        LD_8(z80, z80->A, *z80->readMemory(*z80->DE));
                        z80->cycles += 7;
                        break;

                        case 2:
                        nn = *(uint16_t*)z80->readMemory(*z80->PC+1);
                        val16 = *(uint16_t*)z80->readMemory(nn);
                        *z80->PC += 3;
                        LD_16(z80, rp[2], val16);
                        if(rp[2] == z80->HL)
                            z80->cycles += 16;
                        else
                            z80->cycles += 20;
                        break;

                        case 3:
                        nn = *(uint16_t*)z80->readMemory(*z80->PC+1);
                        val8 = *z80->readMemory(nn);
                        *z80->PC += 3;
                        LD_8(z80, z80->A, val8);
                        z80->cycles += 13;
                        break;
                    }
                    break;
                }
                break;

                case 3:
                switch(q){
                    case 0:
                    *z80->PC += 1;
                    INC_16(z80, rp[p]);
                    break;

                    case 1:
                    *z80->PC += 1;
                    DEC_16(z80, rp[p]);
                    break;
                }
                if(p == 2 && rp[p] != z80->HL)
                    z80->cycles += 10;
                else
                    z80->cycles += 6;
                break;

                case 4:
                *z80->PC += 1;
                if(y == 6 && (prefixDD || prefixFD)){
                    *z80->PC += 1;
                    z80->cycles += 23;
                } else if(y == 6 && !prefixDD && !prefixFD)
                    z80->cycles += 11;
                else if((prefixDD || prefixFD) && (y == 4 || y == 5))
                    z80->cycles += 8;
                else 
                    z80->cycles += 4;
                if(y == 6)
                    r[y] = z80->writeMemory((uintptr_t)r[y]);
                INC_8(z80, r[y]);
                break;

                case 5:
                *z80->PC += 1;
                if(y == 6 && (prefixDD || prefixFD)){
                    *z80->PC += 1;
                    z80->cycles += 23;
                } else if(y == 6 && !prefixDD && !prefixFD)
                    z80->cycles += 11;
                else if((prefixDD || prefixFD) && (y == 4 || y == 5))
                    z80->cycles += 8;
                else 
                    z80->cycles += 4;
                if(y == 6)
                    r[y] = z80->writeMemory((uintptr_t)r[y]);
                DEC_8(z80, r[y]);
                break;

                case 6:
                if(y == 6 && (prefixDD || prefixFD)){
                    adjustDDorFFOpcode(z80, r, rp, rp2);
                    z80->cycles += 19;
                } else if((prefixDD || prefixFD) && (y == 4 || y == 5))
                    z80->cycles += 11;
                else if(y == 6 && !prefixDD && !prefixFD)
                    z80->cycles += 10;
                else
                    z80->cycles += 7;
                if(y == 6)
                    r[y] = z80->writeMemory((uintptr_t)r[y]);
                val8 = *z80->readMemory(*z80->PC+1);
                *z80->PC += 2;
                LD_8(z80, r[y], val8);
                break;

                case 7:
                *z80->PC += 1;
                switch(y){
                    case 0:
                    RLCA(z80);
                    break;

                    case 1:
                    RRCA(z80);
                    break;

                    case 2:
                    RLA(z80);
                    break;

                    case 3:
                    RRA(z80);
                    break;

                    case 4:
                    DAA(z80);
                    break;
                    
                    case 5:
                    CPL(z80);
                    break;

                    case 6:
                    SCF(z80);
                    break;

                    case 7:
                    CCF(z80);
                    break;
                }
                z80->cycles += 4;
                break;
            }
            break;

            case 1:
            *z80->PC += 1;
            if(z == 6 && y == 6){
                HLT(z80);
                z80->cycles += 4;
            } else {
                if((z == 6 || y == 6) && (prefixDD || prefixFD)){
                    adjustDDorFFOpcode(z80, r, rp, rp2);
                    z80->cycles += 19;
                } else if(prefixDD || prefixFD)
                    z80->cycles += 8; 
                else if((z == 6 || y == 6) && !prefixDD && !prefixFD)
                    z80->cycles += 7;
                else
                    z80->cycles += 4;
                if(y == 6)
                    r[y] = z80->writeMemory((uintptr_t)r[y]);
                if(z == 6)
                    r[z] = z80->readMemory((uintptr_t)r[z]);
                LD_8(z80, r[y], *r[z]);
            }
            break;

            case 2:
            if((prefixDD || prefixFD) && z == 6){
                *z80->PC += 1;
                r[z] = z80->readMemory((uintptr_t)r[z]);
                z80->cycles += 19;
            } else if(!prefixDD && !prefixFD && z == 6){
                r[z] = z80->readMemory((uintptr_t)r[z]);
                z80->cycles += 7;
            } else if((prefixDD || prefixFD) && (z == 4 || z == 5))
                z80->cycles += 8;
            else
                z80->cycles += 4;
            aluFunc function = alu[y];
            *z80->PC += 1;
            (*function)(z80, z80->A, *r[z]);
            break;

            case 3:
            switch(z){
                case 0:
                *z80->PC += 1;
                old_PC = *z80->PC;
                switch(y){
                    case 0:
                    RETNZ(z80);
                    break;

                    case 1:
                    RETZ(z80);
                    break;

                    case 2:
                    RETNC(z80);
                    break;

                    case 3:
                    RETC(z80);
                    break;

                    case 4:
                    RETPO(z80);
                    break;

                    case 5:
                    RETPE(z80);
                    break;

                    case 6:
                    RETP(z80);
                    break;

                    case 7:
                    RETM(z80);
                    break;
                }
                if(old_PC == *z80->PC)
                    z80->cycles += 5;
                else
                    z80->cycles += 11;
                break;

                case 1:
                *z80->PC += 1;
                switch(q){
                    case 0:
                    POP(z80, rp2[p]);
                    if(rp2[p] == z80->IX || rp2[p] == z80->IY)
                        z80->cycles += 14;
                    else
                        z80->cycles += 10;
                    break;

                    case 1:
                    switch(p){
                        case 0:
                        RET(z80);
                        z80->cycles += 10;
                        break;

                        case 1:
                        EXX(z80);
                        z80->cycles += 4;
                        break;

                        case 2:
                        JP(z80, *rp[2]);
                        if(rp[2] == z80->HL)
                            z80->cycles += 4;
                        else
                            z80->cycles += 8;
                        break;

                        case 3:
                        LD_16(z80, z80->SP, *rp[2]);
                        if(rp[2] == z80->HL)
                            z80->cycles += 6;
                        else
                            z80->cycles += 10;
                        break;
                    }
                    break;
                }
                break;

                case 2:
                val16 = *(uint16_t*)z80->readMemory(*z80->PC+1);
                *z80->PC += 3;
                switch(y){
                    case 0:
                    JPNZ(z80, val16);
                    break;

                    case 1:
                    JPZ(z80, val16);
                    break;

                    case 2:
                    JPNC(z80, val16);
                    break;

                    case 3:
                    JPC(z80, val16);
                    break;
                    
                    case 4:
                    JPPO(z80, val16);
                    break;

                    case 5:
                    JPPE(z80, val16);
                    break;

                    case 6:
                    JPP(z80, val16);
                    break;

                    case 7:
                    JPM(z80, val16);
                    break;
                }
                z80->cycles += 10;
                break;
            
                case 3:
                switch(y){
                    case 0:
                    val16 = *(uint16_t*)z80->readMemory(*z80->PC+1);
                    *z80->PC += 3;
                    JP(z80, val16);
                    z80->cycles += 10;
                    break;
                    
                    case 2:
                    val8 = *z80->readMemory(*z80->PC+1);
                    *z80->PC += 2;
                    OPCODE_OUT(z80, *z80->A, val8);
                    z80->cycles += 11;
                    break;
                    
                    case 3:
                    val8 = *z80->readMemory(*z80->PC+1);
                    *z80->PC += 2;
                    OPCODE_IN(z80, z80->A, val8);
                    z80->cycles += 11;
                    break;

                    case 4:
                    *z80->PC += 1;
                    uint16_t* addr = (uint16_t*)z80->writeMemory(*z80->SP);
                    EX(z80, addr, rp[2]);
                    if(rp[2] == z80->IX || rp[2] == z80->IY)
                        z80->cycles += 23;
                    else
                        z80->cycles += 19;
                    break;

                    case 5:
                    *z80->PC += 1;
                    EX(z80, z80->DE, z80->HL);
                    z80->cycles += 4;
                    break;

                    case 6:
                    *z80->PC += 1;
                    DI(z80);
                    z80->cycles += 4;
                    break;

                    case 7:
                    *z80->PC += 1;
                    EI(z80);
                    z80->cycles += 4;
                    break;
                }
                break;

                case 4:
                val16 = *(uint16_t*)z80->readMemory(*z80->PC+1);
                *z80->PC += 3;
                old_PC = *z80->PC;
                switch(y){
                    case 0:
                    CALLNZ(z80, val16);
                    break;
                    
                    case 1:
                    CALLZ(z80, val16);
                    break;
                    
                    case 2:
                    CALLNC(z80, val16);
                    break;
                    
                    case 3:
                    CALLC(z80, val16);
                    break;
                    
                    case 4:
                    CALLPO(z80, val16);
                    break;
                    
                    case 5:
                    CALLPE(z80, val16);
                    break;
                    
                    case 6:
                    CALLP(z80, val16);
                    break;
                    
                    case 7:
                    CALLM(z80, val16);
                    break;
                }
                if(old_PC == *z80->PC)
                    z80->cycles += 10;
                else
                    z80->cycles += 17;
                break;

                case 5:
                switch(q){
                    case 0:
                    *z80->PC += 1;
                    PUSH(z80, *rp2[p]);
                    if(rp2[p] == z80->IX || rp2[p] == z80->IY)
                        z80->cycles += 15;
                    else
                        z80->cycles += 11;
                    break;

                    case 1:
                    if(p == 0){
                        val16 = *(uint16_t*)z80->readMemory(*z80->PC+1);
                        *z80->PC += 3;
                        CALL(z80, val16);
                        z80->cycles += 17;
                    }
                    break;
                }
                break;

                case 6:
                val8 = *z80->readMemory(*z80->PC+1);
                aluFunc function = alu[y];
                *z80->PC += 2;
                (*function)(z80, z80->A, val8);
                z80->cycles += 7;
                break;

                case 7:
                *z80->PC += 1;
                RST(z80, y);
                z80->cycles += 11;
                break;
            }
            break;
        }
        break;
    }

}

// Z80 INSTRUCTIONS

void NOP(z80_t* z80){
    z80->cycles += 4;
}

void EX(z80_t* z80, uint16_t* regA, uint16_t* regB){
    uint16_t tmp = *regA;
    *regA = *regB;
    *regB = tmp;
}

void DJNZ(z80_t* z80, int8_t d){
    *z80->B -= 1;   
    if(*z80->B != 0){
        JR(z80, d);  
        z80->cycles += 13;
    } else
        z80->cycles += 8;     
}

void JR(z80_t* z80, int8_t d){
    *z80->PC += d;
}

void JRNZ(z80_t* z80, int8_t d){
    if(!(*z80->F & SET_Z))
        JR(z80, d);
}

void JRZ(z80_t* z80, int8_t d){
    if((*z80->F & SET_Z))
        JR(z80, d);
}

void JRNC(z80_t* z80, int8_t d){
    if(!(*z80->F & SET_C))
        JR(z80, d);
}

void JRC(z80_t* z80, int8_t d){
    if(*z80->F & SET_C)
        JR(z80, d);
}

void LD_8(z80_t* z80, uint8_t* reg, uint8_t val){
    *reg = val;
}

void LD_16(z80_t* z80, uint16_t* reg, uint16_t val){
    *reg = val;
}

void ADD_16(z80_t* z80, uint16_t* regDst, uint16_t* regSrc){
    CLEAR_FLAG(N);
    bool carry = calculateCarry(16, *regDst, *regSrc, 0);
    CHANGE_FLAG(C, carry);

    bool aux_carry = calculateCarry(12, *regDst, *regSrc, 0);
    CHANGE_FLAG(H, aux_carry);

    *regDst += *regSrc;

    CHANGE_FLAG(Y, (*regDst >> 8) & 0b100000);
    CHANGE_FLAG(X, (*regDst >> 8) & 0b1000);
}

void INC_16(z80_t* z80, uint16_t* reg){
    *reg += 1;
}

void DEC_16(z80_t* z80, uint16_t* reg){
    *reg -= 1;
}

void INC_8(z80_t* z80, uint8_t* reg){
    bool aux_carry = calculateCarry(4, *reg, 1, 0);
    CHANGE_FLAG(H, aux_carry);
    CHANGE_FLAG(P, *reg == 0x7f);
    CLEAR_FLAG(N);

    *reg += 1;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);

    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void DEC_8(z80_t* z80, uint8_t* reg){
    bool aux_carry = calculateCarry(4, *reg, -1, 0);
    CHANGE_FLAG(H, !aux_carry);
    CHANGE_FLAG(P, *reg == 0x80);
    SET_FLAG(N);

    *reg -= 1;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);

    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void DAA(z80_t* z80){
    // explanation at:
    //      http://z80-heaven.wikidot.com/instructions-set:daa
    uint8_t correction = 0;
    if((*z80->A & 0X0F) > 0x09 || (*z80->F & SET_H))
        correction += 0x06;

    if((*z80->A > 0x99) || (*z80->F & SET_C)){
        correction += 0x60;
        SET_FLAG(C);
    }

    bool new_half;
    if(*z80->F & SET_N){
        bool old_half = (*z80->F & SET_H);
        new_half = old_half && (*z80->A & 0x0F) < 0x06;
        *z80->A -= correction;
    } else {
        new_half = (*z80->A & 0x0F) > 0x09;
        *z80->A += correction;
    }
    CHANGE_FLAG(H, new_half);

    setSign8Bit(z80, *z80->A);
    setZero(z80, *z80->A);
    setParity(z80, *z80->A);

    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void CPL(z80_t* z80){
    *z80->A = ~(*z80->A);
    SET_FLAG(N);
    SET_FLAG(H);
    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void SCF(z80_t* z80){
    SET_FLAG(C);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);
    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void CCF(z80_t* z80){
    bool carry = *z80->F & SET_C;
    CHANGE_FLAG(C, !carry);
    CHANGE_FLAG(H, carry);
    CLEAR_FLAG(N);
    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void HLT(z80_t* z80){
    z80->HALTED = true;
}

void RETNZ(z80_t* z80){
    if(!(*z80->F & SET_Z))
        POP(z80, z80->PC);
}

void RETZ(z80_t* z80){
    if(*z80->F & SET_Z)
        POP(z80, z80->PC);
}

void RETNC(z80_t* z80){
    if(!(*z80->F & SET_C))
        POP(z80, z80->PC);
}

void RETC(z80_t* z80){
    if(*z80->F & SET_C)
        POP(z80, z80->PC);
}

void RETPO(z80_t* z80){
    if(!(*z80->F & SET_P))
        POP(z80, z80->PC);
}

void RETPE(z80_t* z80){
    if(*z80->F & SET_P)
        POP(z80, z80->PC);
}

void RETP(z80_t* z80){
    if(!(*z80->F & SET_S))
        POP(z80, z80->PC);
}

void RETM(z80_t* z80){
    if(*z80->F & SET_S)
        POP(z80, z80->PC);
}

void POP(z80_t* z80, uint16_t* reg){
    *reg = *(uint16_t*)z80->readMemory(*z80->SP);
    *z80->SP = *z80->SP + 2; 
}

void RET(z80_t* z80){
    POP(z80, z80->PC);
}

void EXX(z80_t* z80){
    EX(z80, z80->BC, z80->BC_);
    EX(z80, z80->DE, z80->DE_);
    EX(z80, z80->HL, z80->HL_);
}

void JP(z80_t* z80, uint16_t val){
    *z80->PC = val;
}

void JPNZ(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_Z))
        JP(z80, val);
}

void JPZ(z80_t* z80, uint16_t val){
    if(*z80->F & SET_Z)
        JP(z80, val);
}

void JPNC(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_C))
        JP(z80, val);
}

void JPC(z80_t* z80, uint16_t val){
    if(*z80->F & SET_C)
        JP(z80, val);
}

void JPPO(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_P))
        JP(z80, val);
}

void JPPE(z80_t* z80, uint16_t val){
    if(*z80->F & SET_P)
        JP(z80, val);
}

void JPP(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_S))
        JP(z80, val);
}

void JPM(z80_t* z80, uint16_t val){
    if(*z80->F & SET_S)
        JP(z80, val);
}

void OPCODE_OUT(z80_t* z80, uint8_t data, uint16_t ioaddr){
    uint8_t* ioport = z80->writeIO(ioaddr);
    *ioport = data;
}

void OPCODE_IN(z80_t* z80, uint8_t* reg, uint16_t ioaddr){
    uint8_t* ioport = z80->readIO(ioaddr);
    *reg = *ioport;
}

void DI(z80_t* z80){
    z80->INTERRUPT_ENABLED = false;
}

void EI(z80_t* z80){
    z80->INTERRUPT_ENABLED = true;
}

void CALL(z80_t* z80, uint16_t val){
    PUSH(z80, *z80->PC);
    *z80->PC = val;
}

void CALLNZ(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_Z))
        CALL(z80, val);
}

void CALLZ(z80_t* z80, uint16_t val){
    if(*z80->F & SET_Z)
        CALL(z80, val);
}

void CALLNC(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_C))
        CALL(z80, val);
}

void CALLC(z80_t* z80, uint16_t val){
    if(*z80->F & SET_C)
        CALL(z80, val);
}

void CALLPO(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_P))
        CALL(z80, val);
}

void CALLPE(z80_t* z80, uint16_t val){
    if(*z80->F & SET_P)
        CALL(z80, val);
}

void CALLP(z80_t* z80, uint16_t val){
    if(!(*z80->F & SET_S))
        CALL(z80, val);
}

void CALLM(z80_t* z80, uint16_t val){
    if(*z80->F & SET_S)
        CALL(z80, val);
}

void PUSH(z80_t* z80, uint16_t val){
    *z80->SP = *z80->SP - 2;
    *(uint16_t*)z80->writeMemory(*z80->SP) = val;
}

void RST(z80_t* z80, uint8_t addr){
    CALL(z80, addr*8);
}

void RLCA(z80_t* z80){
    bool carry = *z80->A & 0b10000000;
    CHANGE_FLAG(C, carry);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    *z80->A = (*z80->A << 1) | (carry);
    
    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void RRCA(z80_t* z80){
    bool carry = *z80->A & 0b1;
    CHANGE_FLAG(C, carry);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);
    *z80->A = (*z80->A >> 1) | (carry << 7);

    CHANGE_FLAG(Y, *z80->A & 0b100000);
    CHANGE_FLAG(X, *z80->A & 0b1000);
}

void RLA(z80_t* z80){
    bool carry = *z80->F & SET_C;
    CHANGE_FLAG(C, *z80->A & 0b10000000);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);
    *z80->A = (*z80->A << 1) | carry;

    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void RRA(z80_t* z80){
    bool carry = *z80->F & SET_C;
    CHANGE_FLAG(C, *z80->A & 0b1);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);
    *z80->A = (*z80->A >> 1) | (carry << 7);

    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void RLC(z80_t* z80, uint8_t* reg){
    bool msb = *reg >> 7;
    CHANGE_FLAG(C, msb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);
    *reg = (*reg << 1) | msb;
    
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    setParity(z80, *reg);
    
    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void RRC(z80_t* z80, uint8_t* reg){
    bool lsb = *reg & 0b1;
    CHANGE_FLAG(C, lsb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);
    *reg = (lsb << 7) | (*reg >> 1);

    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    setParity(z80, *reg);
    
    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void RL(z80_t* z80, uint8_t* reg){
    bool carry = *z80->F & SET_C;
    bool msb = *reg >> 7;
    *reg = (*reg << 1) | carry;
    CHANGE_FLAG(C, msb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    setParity(z80, *reg);
    
    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void RR(z80_t* z80, uint8_t* reg){
    bool carry = *z80->F & SET_C;
    bool lsb = *reg & 0b1;
    *reg = (*reg >> 1) | (carry << 7);
    CHANGE_FLAG(C, lsb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    setParity(z80, *reg);

    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void SLA(z80_t* z80, uint8_t* reg){
    bool msb = *reg >> 7;
    CHANGE_FLAG(C, msb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    *reg = *reg << 1;

    setParity(z80, *reg);
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    
    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void SRA(z80_t* z80, uint8_t* reg){
    bool sign = *reg >> 7;
    bool lsb = *reg & 0b1;
    CHANGE_FLAG(C, lsb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    *reg = (*reg >> 1) | (sign << 7);

    setParity(z80, *reg);
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);

    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void SLL(z80_t* z80, uint8_t* reg){
    bool msb = *reg >> 7;
    CHANGE_FLAG(C, msb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    *reg = *reg << 1;
    *reg = *reg | 0b1;

    setParity(z80, *reg);
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);

    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}


void SRL(z80_t* z80, uint8_t* reg){
    bool lsb = *reg & 0b1;
    CHANGE_FLAG(C, lsb);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    *reg = *reg >> 1;
   
    setParity(z80, *reg);
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    
    CHANGE_FLAG(X, *reg & 0b1000);
    CHANGE_FLAG(Y, *reg & 0b100000);
}

void BIT(z80_t* z80, uint8_t bit, uint8_t* reg){
    uint8_t masked_bit = *reg & (1 << bit);
    CHANGE_FLAG(Z, masked_bit == 0);
    CHANGE_FLAG(P, masked_bit == 0);
    CHANGE_FLAG(S, masked_bit & 0x80);
    SET_FLAG(H);
    CLEAR_FLAG(N);
    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void RES(z80_t* z80, uint8_t bit, uint8_t* reg){
    *reg = *reg & (~(uint8_t)(1 << bit));
}

void SET(z80_t* z80, uint8_t bit, uint8_t* reg){
    *reg = * reg | (1 << bit);
}

void ADD(z80_t* z80, uint8_t* reg, uint8_t val){
    uint8_t res = *reg + val;
    bool carry = calculateCarry(8, *reg, val, 0);
    CHANGE_FLAG(C, carry);

    bool aux_carry = calculateCarry(4, *reg, val, 0);
    CHANGE_FLAG(H, aux_carry);
    
    bool overflow = calculateCarry(7, *reg, val, 0) != calculateCarry(8, *reg, val, 0);
    CHANGE_FLAG(P, overflow);

    *reg = res;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    CLEAR_FLAG(N);

    CHANGE_FLAG(Y, res & 0b100000);
    CHANGE_FLAG(X, res & 0b1000);
}

void ADC(z80_t* z80, uint8_t* reg, uint8_t val){
    bool carry = *z80->F & SET_C;
    uint8_t res = *reg + val + carry;

    bool new_carry = calculateCarry(8, *reg, val, carry);
    CHANGE_FLAG(C, new_carry);

    bool aux_carry = calculateCarry(4, *reg, val, carry);
    CHANGE_FLAG(H, aux_carry);
    
    bool overflow = calculateCarry(7, *reg, val, carry) != calculateCarry(8, *reg, val, carry);
    CHANGE_FLAG(P, overflow);

    *reg = res;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    CLEAR_FLAG(N);

    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void SUB(z80_t* z80, uint8_t* reg, uint8_t val){
    val = ~val + 1;
    bool carry = calculateCarry(8, *reg, val - 1, 1);
    CHANGE_FLAG(C, !carry);

    bool aux_carry = calculateCarry(4, *reg, val - 1, 1);
    CHANGE_FLAG(H, !aux_carry);

    uint8_t res = *reg + val;

    bool overflow = calculateCarry(7, *reg, val - 1, 1) != calculateCarry(8, *reg, val - 1, 1);
    CHANGE_FLAG(P, overflow);

    *reg = res;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    SET_FLAG(N);

    CHANGE_FLAG(Y, res & 0b100000);
    CHANGE_FLAG(X, res & 0b1000);
}

void SBC(z80_t* z80, uint8_t* reg, uint8_t val){
    val = ~val + 1;
    bool carry = *z80->F & SET_C;
    bool new_carry = calculateCarry(8, *reg, val - 1, !carry);
    bool aux_carry = calculateCarry(4, *reg, val - 1, !carry);

    CHANGE_FLAG(C, !new_carry);
    CHANGE_FLAG(H, !aux_carry);

    uint8_t res = *reg + val - carry;
    bool overflow = calculateCarry(7, *reg, val - 1, !carry) != calculateCarry(8, *reg, val - 1, !carry);
    CHANGE_FLAG(P, overflow); 

    *reg = res;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    SET_FLAG(N);

    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void AND(z80_t* z80, uint8_t* reg, uint8_t val){
    *reg &= val;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    SET_FLAG(H);
    CLEAR_FLAG(C);
    CLEAR_FLAG(N);
    setParity(z80, *reg);

    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void XOR(z80_t* z80, uint8_t* reg, uint8_t val){
    *reg ^= val;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    setParity(z80, *reg);
    CLEAR_FLAG(H);
    CLEAR_FLAG(C);
    CLEAR_FLAG(N);
    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void OR(z80_t* z80, uint8_t* reg, uint8_t val){
    *reg |= val;
    setSign8Bit(z80, *reg);
    setZero(z80, *reg);
    setParity(z80, *reg);
    CLEAR_FLAG(H);
    CLEAR_FLAG(C);
    CLEAR_FLAG(N);
    CHANGE_FLAG(Y, *reg & 0b100000);
    CHANGE_FLAG(X, *reg & 0b1000);
}

void CP(z80_t* z80, uint8_t* reg, uint8_t val){
    uint8_t copy = *reg;
    SUB(z80, reg, val);
    *reg = copy;

    CHANGE_FLAG(Y, val & 0b100000);
    CHANGE_FLAG(X, val & 0b1000);
}

void ADC_16(z80_t* z80, uint16_t* reg, uint16_t val){
    bool carry = *z80->F & SET_C;

    bool new_carry = calculateCarry(16, *reg, val, carry);
    CHANGE_FLAG(C, new_carry);
    
    bool aux_carry = calculateCarry(12, *reg, val, carry);
    CHANGE_FLAG(H, aux_carry);

    uint16_t res = *reg + val + carry;
    bool overflow = calculateCarry(16, *reg, val, carry) != calculateCarry(15, *reg, val, carry);
    CHANGE_FLAG(P, overflow);
           
    *reg = res;
    setSign16Bit(z80, *reg);
    setZero(z80, *reg);
    CLEAR_FLAG(N);

    CHANGE_FLAG(Y, (res >> 8) & 0b100000);
    CHANGE_FLAG(X, (res >> 8) & 0b1000);
}

void SBC_16(z80_t* z80, uint16_t* reg, uint16_t val){
    val = ~val + 1;
    bool carry = *z80->F & SET_C;
    bool new_carry = calculateCarry(16, *reg, val - 1, !carry);
    bool aux_carry = calculateCarry(12, *reg, val - 1, !carry);

    CHANGE_FLAG(C, !new_carry);
    CHANGE_FLAG(H, !aux_carry);

    uint16_t res = *reg + val - carry;
    bool overflow = calculateCarry(16, *reg, val - 1, !carry) != calculateCarry(15, *reg, val - 1, !carry);
    CHANGE_FLAG(P, overflow);  

    *reg = res;
    setSign16Bit(z80, *reg);
    setZero(z80, *reg);
    SET_FLAG(N);

    CHANGE_FLAG(Y, (res >> 8) & 0b100000);
    CHANGE_FLAG(X, (res >> 8) & 0b1000);
}

void NEG(z80_t* z80, uint8_t* reg){
    uint8_t tmp = *reg;
    *reg = 0;
    SUB(z80, reg, tmp);
}

void RETI(z80_t* z80){
    RET(z80);
}

void RETN(z80_t* z80){
    RET(z80);
}

void IM(z80_t* z80, uint8_t im_mode){
    z80->INTERRUPT_MODE = im_mode;
}

void RRD(z80_t* z80){
    uint8_t tmpA = *z80->A;
    uint8_t val = *z80->readMemory(*z80->HL);
    *z80->A = (tmpA & 0xF0) | (val & 0xF);
    *z80->writeMemory(*z80->HL) = (val >> 4) | (tmpA << 4);
    setSign8Bit(z80, *z80->A);
    setZero(z80, *z80->A);
    setParity(z80, *z80->A);
    CLEAR_FLAG(N);
    CLEAR_FLAG(H);
    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

void RLD(z80_t* z80){
    uint8_t tmpA = *z80->A;
    uint8_t val = *z80->readMemory(*z80->HL);
    *z80->A = (tmpA & 0xF0) | (val >> 4);
    *z80->writeMemory(*z80->HL) = (val << 4) | (tmpA & 0xF);
    setSign8Bit(z80, *z80->A);
    setZero(z80, *z80->A);
    setParity(z80, *z80->A);
    CLEAR_FLAG(N);
    CLEAR_FLAG(H);
    CHANGE_FLAG(X, *z80->A & 0b1000);
    CHANGE_FLAG(Y, *z80->A & 0b100000);
}

// block instructions

void LDI(z80_t* z80){
    uint8_t hl_val = *z80->readMemory(*z80->HL);
    *z80->writeMemory(*z80->DE) = hl_val;
    *z80->DE += 1;
    *z80->HL += 1;
    *z80->BC -= 1;

    CHANGE_FLAG(P, *z80->BC != 0);
    CLEAR_FLAG(H);
    CLEAR_FLAG(N);

    z80->cycles += 16;

    uint8_t res = *z80->A + hl_val;
    CHANGE_FLAG(X, res & 0b1000);
    CHANGE_FLAG(Y, res & 0b10);
}

void LDD(z80_t* z80){
    LDI(z80);
    *z80->HL -= 2;
    *z80->DE -= 2;
}

void LDIR(z80_t* z80){
    LDI(z80);
    z80->cycles -= 16;
    if(*z80->BC != 0){
        *z80->PC -= 2;
        SET_FLAG(P);
        z80->cycles += 21;
    } else {
        CLEAR_FLAG(P);
        z80->cycles += 16;
    }
}

void LDDR(z80_t* z80){
    LDD(z80);
    z80->cycles -= 16;
    if(*z80->BC != 0){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void CPI(z80_t* z80){
    bool carry = (bool)(*z80->F & SET_C);
    uint8_t memory_val = *z80->readMemory(*z80->HL);
    CP(z80, z80->A, memory_val);
    *z80->HL += 1;
    *z80->BC -= 1;

    CHANGE_FLAG(P, *z80->BC != 0);
    CHANGE_FLAG(C, carry);
    SET_FLAG(N);

    bool aux_carry = (bool)(*z80->F & SET_H);
    uint8_t val = *z80->A - memory_val - aux_carry;
    
    CHANGE_FLAG(Y, val & 0b10);
    CHANGE_FLAG(X, val & 0b1000);

    z80->cycles += 16;
}

void CPD(z80_t* z80){
    CPI(z80);
    *z80->HL -= 2;
}

void CPIR(z80_t* z80){
    CPI(z80);
    z80->cycles -= 16;
    if(*z80->BC != 0 && !(bool)(*z80->F & SET_Z)){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void CPDR(z80_t* z80){
    CPD(z80);
    z80->cycles -= 16;
    if(*z80->BC != 0 && !(bool)(*z80->F & SET_Z)){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void INI(z80_t* z80){
    *z80->B -= 1;
    OPCODE_IN(z80, z80->writeMemory(*z80->HL), *z80->BC); 
    *z80->HL += 1;  
    z80->cycles += 16;
}

void IND(z80_t* z80){
    *z80->B -= 1;  
    OPCODE_IN(z80, z80->writeMemory(*z80->HL), *z80->BC); 
    *z80->HL -= 1;
    z80->cycles += 16;
}

void INIR(z80_t* z80){
    INI(z80);
    z80->cycles -= 16;
    if(*z80->B != 0){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void INDR(z80_t* z80){
    IND(z80);
    z80->cycles -= 16;
    if(*z80->B != 0){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void OUTI(z80_t* z80){
    *z80->B -= 1;
    OPCODE_OUT(z80, *z80->readMemory(*z80->HL), *z80->BC);
    *z80->HL += 1;
    z80->cycles += 16;
}

void OUTD(z80_t* z80){
    *z80->B -= 1;
    OPCODE_OUT(z80, *z80->readMemory(*z80->HL), *z80->BC);
    *z80->HL -= 1;
    z80->cycles += 16;
}

void OTIR(z80_t* z80){
    OUTI(z80);
    z80->cycles -= 16;
    if(*z80->B != 0){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void OTDR(z80_t* z80){
    OUTD(z80);
    z80->cycles -= 16;
    if(*z80->B != 0){
        *z80->PC -= 2;
        z80->cycles += 21;
    } else {
        z80->cycles += 16;
    }
}

void setParity(z80_t* z80, uint8_t val){
    int counter = 0;
    while(val != 0){
        counter += (val & 0x1);
        val = val >> 1;
    }
    bool parity = (counter % 2 == 0);
    CHANGE_FLAG(P, parity);
}

void setZero(z80_t* z80, uint16_t val){
    CHANGE_FLAG(Z, val == 0);
}

void setSign8Bit(z80_t* z80, uint8_t val){
    CHANGE_FLAG(S, val & 0x80);
}

void setSign16Bit(z80_t* z80, uint16_t val){
    CHANGE_FLAG(S, val & 0x8000);
}

bool calculateCarry(int bit, uint16_t a, uint16_t b, bool cy) {
  int32_t result = a + b + cy;
  int32_t carry = result ^ a ^ b;
  return carry & (1 << bit);
}