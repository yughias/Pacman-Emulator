#include <hardware.h>
#include <cpu_utility.h>

// check if cpu is working
bool HALTED;
bool INTERRUPT_ENABLED;
bool INTERRUPT_PENDING;
uint16_t INTERRUPT_VECT;

// registers in 16 bit mode
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

// 8 bit registers of IX and IY
uint8_t* IXL;
uint8_t* IXH;
uint8_t* IYL;
uint8_t* IYH;

// registers in 8 bit mode
uint8_t* A;
uint8_t* F;
uint8_t* B;
uint8_t* C;
uint8_t* D;
uint8_t* E;
uint8_t* H;
uint8_t* L;

uint8_t I[1];
uint8_t R[1];

// other 16 bit registers
uint16_t SP[1];
uint16_t PC[1];

// cpuCycles
uint64_t cpuCycles;

typedef void (*rotateFunc)(uint8_t*);
typedef void (*aluFunc)(uint8_t*, uint8_t);
typedef void (*blockFunc)();

rotateFunc rot[8];
aluFunc alu[8];
blockFunc bli[16];
uint16_t* rp[4];
uint16_t* rp2[4];
uint8_t* r[8];


void initCPU(){
    cpuCycles = 0;
    F = (uint8_t*)AF;
    A = F + 1;
    C = (uint8_t*)BC;
    B = C + 1;
    E = (uint8_t*)DE;
    D = E + 1;
    L = (uint8_t*)HL;
    H = L + 1;

    IXL = (uint8_t*)IX;
    IXH = IXL + 1;

    IYL = (uint8_t*)IY;
    IYH = IYL + 1;
    // WARNING: normally registers could have everything inside of it
    // but for clarity, we assume they are empty
    // set everything to 0
    *AF = 0xFFFF;
    *BC = 0;
    *DE = 0;
    *HL = 0;
    *SP = 0xFFFF;
    *PC = 0;

    *AF_ = 0;
    *BC_ = 0;
    *DE_ = 0;
    *HL_ = 0;
    // set default bit of the flag register
    *I = 0;
    *R = 0;
    HALTED = false;
    INTERRUPT_ENABLED = false;
    INTERRUPT_PENDING = false;
    INTERRUPT_VECT = 0x0000;

    // set hash table for decoding
    restoreRegisterTable();

    rot[0] = RLC;
    rot[1] = RRC;
    rot[2] = RL;
    rot[3] = RR;
    rot[4] = SLA;
    rot[5] = SRA;
    rot[6] = SLL;
    rot[7] = SRL;

    alu[0] = ADD;
    alu[1] = ADC;
    alu[2] = SUB;
    alu[3] = SBC;
    alu[4] = AND;
    alu[5] = XOR;
    alu[6] = OR;
    alu[7] = CP;

    bli[0] = LDI;
    bli[1] = LDD;
    bli[2] = LDIR;
    bli[3] = LDDR;
    bli[4] = CPI;
    bli[5] = CPD;
    bli[6] = CPIR;
    bli[7] = CPDR;
    bli[8] = INI;
    bli[9] = IND;
    bli[10] = INIR;
    bli[11] = INDR;
    bli[12] = OUTI;
    bli[13] = OUTD;
    bli[14] = OTIR;
    bli[15] = OTDR;
}

void restoreRegisterTable(){
    rp[0] = BC;
    rp[1] = DE;
    rp[2] = HL;
    rp[3] = SP;

    rp2[0] = BC;
    rp2[1] = DE;
    rp2[2] = HL;
    rp2[3] = AF;

    r[0] = B;
    r[1] = C;
    r[2] = D;
    r[3] = E;
    r[4] = H;
    r[5] = L;
    r[6] = MEMORY + *HL;
    r[7] = A;
}

void useDDRegisterTable(){
    rp[2] = IX;

    rp2[2] = IX;

    r[4] = IXH;
    r[5] = IXL;
    r[6] = MEMORY+((int16_t)*IX+((int8_t)MEMORY[*PC+2]));
}

void useFDRegisterTable(){
    rp[2] = IY;

    rp2[2] = IY;

    r[4] = IYH;
    r[5] = IYL;
    r[6] = MEMORY+((int16_t)*IY+((int8_t)MEMORY[*PC+2]));
}

void adjustDDorFFOpcode(){
    *PC += 1;
    r[4] = H;
    r[5] = L;
}

void infoCPU(){
    fprintf(stderr, "%llu ", cpuCycles);
    fprintf(stderr, "$%04X\t", *PC);
    fprintf(stderr, "AF :0x%04X BC: 0x%04X ", *AF, *BC);
    fprintf(stderr, "DE: 0x%04X HL: 0x%04X\n", *DE, *HL);
    fprintf(stderr, "AF_ :0x%04X BC_: 0x%04X DE_: 0x%04X HL_: 0x%04X\n", *AF_, *BC_, *DE_, *HL_);
    fprintf(stderr, "IX: 0x%04X IY: 0x%04X\n", *IX, *IY);
    fprintf(stderr, "S: %d ", (bool)(*F & SET_S));
    fprintf(stderr, "Z: %d ", (bool)(*F & SET_Z));
    fprintf(stderr, "C: %d ", (bool)(*F & SET_C));
    fprintf(stderr, "P: %d ", (bool)(*F & SET_P));
    fprintf(stderr, "A: %d ", (bool)(*F & SET_H));
    fprintf(stderr, "SP: 0x%04X\n", *SP);
    fprintf(stderr, "Stack: 0x%04X\n", *(uint16_t*)(MEMORY+*SP));
    fprintf(stderr, "OPCODE: 0x%02X %02X %02X", MEMORY[*PC], MEMORY[*PC+1], MEMORY[*PC+2]);
    fprintf(stderr, "\n\n");
}

void processInterrupt(){
    if(INTERRUPT_ENABLED && INTERRUPT_PENDING){
            INTERRUPT_ENABLED = false;
            INTERRUPT_PENDING = false;
            HALTED = false;
            uint16_t interruptAddress = *(uint16_t*)(MEMORY + ((*I << 8) | INTERRUPT_VECT));
            CALL(interruptAddress);
    }
}

void stepCPU(){
    if(INTERRUPT_ENABLED && INTERRUPT_PENDING){
        processInterrupt();
        return;
    }

    if(HALTED)
        return;
        
    #ifdef DEBUG
        infoCPU();
    #endif
    uint8_t opcode = MEMORY[*PC];
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t q;
    uint8_t p;
    bool prefixDD = false;
    bool prefixFD = false;

    restoreRegisterTable();
    if(opcode == 0xDD){
        useDDRegisterTable();
        prefixDD = true;
        *PC += 1;
    } else if(opcode == 0XFD){
        useFDRegisterTable();
        prefixFD = true;
        *PC += 1;
    }

    opcode = MEMORY[*PC];

    if(opcode == 0xCB){
        if(prefixDD || prefixFD)
            *PC += 1;

        *PC += 1;
        opcode = MEMORY[*PC];
        x = opcode >> 6;
        y = (opcode >> 3) & 0b111;
        z = opcode & 0b111;
        q = y & 0b1;
        p = (y >> 1) & 0b11;

        *PC += 1;

        if(x == 0){
            rotateFunc function = rot[y];
            uint8_t* operand;
            operand = r[z];
            (*function)(operand);
        }
        if(z == 6){
            if(x == 1)
                r[z] = getReadAddress(r[z] - MEMORY);
            else
                r[z] = getWriteAddress(r[z] - MEMORY);
        }
        if(x == 1){
            BIT(y, r[z]);    
        }
        if(x == 2){
            RES(y, r[z]);
        }
        if(x == 3){
            SET(y, r[z]);
        }
    } else if(opcode == 0xED){
        // ED OPCODE TABLE

        *PC = *PC + 1;
        opcode = MEMORY[*PC];
        x = opcode >> 6;
        y = (opcode >> 3) & 0b111;
        z = opcode & 0b111;
        q = y & 0b1;
        p = (y >> 1) & 0b11;

        if(x == 0 || x == 3){
            *PC += 1;
            NOP();
        }
        if(x == 1){
            if(z == 0){
                *PC += 1;
                if(y != 6){
                    IN(r[y], IO[*C]);
                    setSign8Bit(*(r[y]));
                    setParity(*(r[y]));
                    setSign8Bit(*r[y]);
                } else {
                    uint8_t copy = *A;
                    IN(A, IO[*C]);
                    setSign8Bit(*A);
                    setParity(*A);
                    setSign8Bit(*A);
                    *A = copy;
                }
            }
            if(z == 1){
                *PC += 1;
                if(y != 6){
                    OUT(IO+*C, *(r[y]));
                    setSign8Bit(*(r[y]));
                    setParity(*(r[y]));
                    setSign8Bit(*(r[y]));
                } else {
                    OUT(IO+*C, 0);
                    setSign8Bit(*A);
                    setParity(*A);
                    setSign8Bit(*A);
                }
            }
            if(z == 2){
                *PC += 1;
                if(q == 0){
                    SBC_16(HL, *(rp[p]));
                }
                if(q == 1){
                    ADC_16(HL, *(rp[p]));
                }
            }
            if(z == 3){
                uint16_t nn = *(uint16_t*)(MEMORY+*PC+1);
                *PC += 3;
                if(q == 0){
                    LD_16((uint16_t*)getWriteAddress(nn), *(rp[p]));
                }
                if(q == 1){
                    LD_16(rp[p], *(uint16_t*)getReadAddress(nn));
                }
            }
            if(z == 4){
                *PC += 1;
                NEG(A);
            }
            if(z == 5){
                *PC += 1;
                if(y != 1){
                    RETN();
                }
                if(y == 1){
                    RETI();
                }
            }
            if(z == 6){
                *PC += 1;
                IM();
            }
            if(z == 7){
                *PC += 1;
                if(y == 0){
                    LD_8(I, *A);
                }
                if(y == 1){
                    LD_8(R, *A);
                }
                if(y == 2){
                    LD_8(A, *I);
                }
                if(y == 3){
                    LD_8(A, *R);
                }
                if(y == 4){
                    RRD();
                }
                if(y == 5){
                    RLD();
                }
                if(y == 6){
                    NOP();
                }
                if(y == 7){
                    NOP();
                }
            }
        }
        if(x == 2){
            *PC += 1;
            if(z <= 3 && y >= 4){
                blockFunc function = bli[(y-4)+z*4];
                (*function)();
            }
        }
    } else {
        // NORMAL OPCODE TABLE

        opcode = MEMORY[*PC];
        x = opcode >> 6;
        y = (opcode >> 3) & 0b111;
        z = opcode & 0b111;
        q = y & 0b1;
        p = (y >> 1) & 0b11;

        if(x == 0){
            if(z == 0){
                if(y == 0){
                    *PC += 1;
                    NOP();
                }
                if(y == 1){
                    *PC += 1;
                    EX(AF, AF_);
                    cpuCycles = 4;
                }
                if(y == 2){
                    *PC += 2;
                    DJNZ(MEMORY[*PC-1]);
                }   
                if(y == 3){
                    *PC += 2;
                    JR(MEMORY[*PC-1]);
                }
                if(y == 4){
                    *PC += 2;
                    JRNZ(MEMORY[*PC-1]);
                }
                if(y == 5){
                    *PC += 2;
                    JRZ(MEMORY[*PC-1]);
                }
                if(y == 6){
                    *PC += 2;
                    JRNC(MEMORY[*PC-1]);
                }
                if(y == 7){
                    *PC += 2;
                    JRC(MEMORY[*PC-1]);
                }
            }
            if(z == 1){
                if(q == 0){
                    uint16_t nn = *(uint16_t*)(MEMORY+*PC+1);
                    *PC += 3;
                    LD_16(rp[p], nn);
                }
                if(q == 1){
                    *PC += 1;
                    ADD_16(rp[2], rp[p]);
                }
            }
            if(z == 2){
                if(q == 0){
                    if(p == 0){
                        *PC += 1;
                        LD_8(getWriteAddress(*BC), *A);
                    }
                    if(p == 1){
                        *PC += 1;
                        LD_8(getWriteAddress(*DE), *A);
                    }
                    if(p == 2){
                        uint16_t nn = *(uint16_t*)(MEMORY+*PC+1);
                        *PC += 3;
                        LD_16((uint16_t*)getWriteAddress(nn), *(rp[2]));
                    }
                    if(p == 3){
                        uint16_t nn = *(uint16_t*)(MEMORY+*PC+1);
                        *PC += 3;
                        LD_8((uint8_t*)getWriteAddress(nn), *A);
                    }
                }
                if(q == 1){
                    if(p == 0){
                        *PC += 1;
                        LD_8(A, *getReadAddress(*BC));
                    }
                    if(p == 1){
                        *PC += 1;
                        LD_8(A, *getReadAddress(*DE));
                    }
                    if(p == 2){
                        uint16_t nn = *(uint16_t*)(MEMORY+*PC+1);
                        uint16_t val = *(uint16_t*)getReadAddress(nn);
                        *PC += 3;
                        LD_16(rp[2], val);
                    }
                    if(p == 3){
                        uint16_t nn = *(uint16_t*)(MEMORY+*PC+1);
                        uint8_t val = *getReadAddress(nn);
                        *PC += 3;
                        LD_8(A, val);
                    }
                }
            }
            if(z == 3){
                if(q == 0){
                    *PC += 1;
                    INC_16(rp[p]);
                }
                if(q == 1){
                    *PC += 1;
                    DEC_16(rp[p]);
                }
            }
            if(z == 4){
                *PC += 1;
                if(y == 6 && (prefixDD || prefixFD))
                    *PC += 1;
                if(y == 6)
                    r[y] = getWriteAddress(r[y] - MEMORY);
                INC_8(r[y]);
            }
            if(z == 5){
                *PC += 1;
                if(y == 6 && (prefixDD || prefixFD))
                    *PC += 1;
                if(y == 6)
                    r[y] = getWriteAddress(r[y] - MEMORY);
                DEC_8(r[y]);
            }
            if(z == 6){
                if(y == 6 && (prefixDD || prefixFD))
                    adjustDDorFFOpcode();
                if(y == 6)
                    r[y] = getWriteAddress(r[y] - MEMORY);
                uint8_t val = MEMORY[*PC+1];
                *PC += 2;
                LD_8(r[y], val);
            }
            if(z == 7){
                *PC += 1;
                if(y == 0){
                    RLCA();
                }
                if(y == 1){
                    RRCA();
                }
                if(y == 2){
                    RLA();
                }
                if(y == 3){
                    RRA();
                }
                if(y == 4){
                    DAA();
                }
                if(y == 5){
                    CPL();
                }
                if(y == 6){
                    SCF();
                }
                if(y == 7){
                    CCF();
                }
            }
        }
        if(x == 1){
            *PC += 1;
            if(z == 6 && y == 6){
                HLT();
            } else {
                if((z == 6 || y == 6) && (prefixDD || prefixFD))
                    adjustDDorFFOpcode();
                if(y == 6)
                    r[y] = getWriteAddress(r[y] - MEMORY);
                if(z == 6)
                    r[z] = getReadAddress(r[z] - MEMORY);
                LD_8(r[y], *(r[z]));
            }
        }
        if(x == 2){
            if((prefixDD || prefixFD) && z == 6){
                *PC += 1;
                r[z] = getReadAddress(r[z] - MEMORY);
            }
            aluFunc function = alu[y];
            *PC += 1;
            (*function)(A, *(r[z]));
        }
        if(x == 3){
            if(z == 0){
                *PC += 1;
                if(y == 0){
                    RETNZ();
                }
                if(y == 1){
                    RETZ();
                }
                if(y == 2){
                    RETNC();
                }
                if(y == 3){
                    RETC();
                }
                if(y == 4){
                    RETPO();
                }
                if(y == 5){
                    RETPE();
                }
                if(y == 6){
                    RETP();
                }
                if(y == 7){
                    RETM();
                }
            }
            if(z == 1){
                *PC += 1;
                if(q == 0){
                    POP(rp2[p]);
                    if(rp2[p] == IX || rp2[p] == IY)
                        cpuCycles = 14;
                    else
                        cpuCycles = 10;
                }
                if(q == 1){
                    if(p == 0){
                        RET();
                    }
                    if(p == 1){
                        EXX();
                        cpuCycles = 4;
                    }
                    if(p == 2){
                        JP(*(rp[2]));
                    }
                    if(p == 3){
                        LD_16(SP, *(rp[2]));
                    }
                }
            }
            if(z == 2){
                uint16_t val = *(uint16_t*)(MEMORY+*PC+1);
                *PC += 3;
                if(y == 0){
                    JPNZ(val);
                }
                if(y == 1){
                    JPZ(val);
                }
                if(y == 2){
                    JPNC(val);
                }
                if(y == 3){
                    JPC(val);
                }
                if(y == 4){
                    JPPO(val);
                }
                if(y == 5){
                    JPPE(val);
                }
                if(y == 6){
                    JPP(val);
                }
                if(y == 7){
                    JPM(val);
                }
            }
            if(z == 3){
                if(y == 0){
                    uint16_t val = *(uint16_t*)(MEMORY+*PC+1);
                    *PC += 3;
                    JP(val);
                }
                if(y == 2){
                    uint8_t ioaddr = MEMORY[*PC+1];
                    *PC += 2;
                    OUT(IO+ioaddr, *A);
                }
                if(y == 3){
                    uint8_t ioaddr = MEMORY[*PC+1];
                    *PC += 2;
                    IN(A, IO[ioaddr]);
                }
                if(y == 4){
                    *PC += 1;
                    uint16_t* addr = (uint16_t*)getWriteAddress(*SP);
                    EX(addr, rp[2]);
                    if(rp[2] == IX || rp[2] == IY)
                        cpuCycles = 23;
                    else
                        cpuCycles = 19;
                }
                if(y == 5){
                    *PC += 1;
                    EX(DE, HL);
                    cpuCycles = 4;
                }
                if(y == 6){
                    *PC += 1;
                    DI();
                }
                if(y == 7){
                    *PC += 1;
                    EI();
                }
            }
            if(z == 4){
                uint16_t val = *(uint16_t*)(MEMORY+*PC+1);
                *PC += 3;
                if(y == 0){
                    CALLNZ(val);
                }
                if(y == 1){
                    CALLZ(val);
                }
                if(y == 2){
                    CALLNC(val);
                }
                if(y == 3){
                    CALLC(val);
                }
                if(y == 4){
                    CALLPO(val);
                }
                if(y == 5){
                    CALLPE(val);
                }
                if(y == 6){
                    CALLP(val);
                }
                if(y== 7){
                    CALLM(val);
                }
            }
            if(z == 5){
                if(q == 0){
                    *PC += 1;
                    PUSH(*(rp2[p]));
                    if(rp2[p] == IX || rp2[p] == IY)
                        cpuCycles = 15;
                    else
                        cpuCycles = 11;
                }
                if(q == 1){
                    if(p == 0){
                        uint16_t val = *(uint16_t*)(MEMORY+*PC+1);
                        *PC += 3;
                        CALL(val);
                    }
                }
            }
            if(z == 6){
                uint8_t val = MEMORY[*PC+1];
                aluFunc function = alu[y];
                *PC += 2;
                (*function)(A, val);
            }
            if(z == 7){
                *PC += 1;
                RST(y);
            }
        }
    }
   
}

//z80 NEW INSTRUCTION

void NOP(){
}

void EX(uint16_t* regA, uint16_t* regB){
    uint16_t tmp = *regA;
    *regA = *regB;
    *regB = tmp;
}

void DJNZ(int8_t d){
    *B = *B - 1;   
    if(*B != 0)
        JR(d);       
}

void JR(int8_t d){
    *PC += d;
}

void JRNZ(int8_t d){
    if(!(*F & SET_Z))
        JR(d);
}

void JRZ(int8_t d){
    if((*F & SET_Z))
        JR(d);
}

void JRNC(int8_t d){
    if(!(*F & SET_C))
        JR(d);
}

void JRC(int8_t d){
    if(*F & SET_C)
        JR(d);
}

void LD_8(uint8_t* reg, uint8_t val){
    *reg = val;
}

void LD_16(uint16_t* reg, uint16_t val){
    *reg = val;
}

void ADD_16(uint16_t* regDst, uint16_t* regSrc){
    *F &= CLEAR_N;
    if(*regDst + *regSrc > 0xffff)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    bool aux_carry = calculateCarry(11, *regDst, *regSrc, 0);
    if(aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    *regDst += *regSrc;
}

void INC_16(uint16_t* reg){
    *reg += 1;
}

void DEC_16(uint16_t* reg){
    *reg -= 1;
}

void INC_8(uint8_t* reg){
    bool aux_carry = calculateCarry(4, *reg, 1, 0);
    if(aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    if(*reg == 0x7f)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    *reg += 1;
    *F &= CLEAR_N;
    setSign8Bit(*reg);
    setZero(*reg);
}

void DEC_8(uint8_t* reg){
    bool aux_carry = calculateCarry(4, *reg, -1, 0);
    if(!aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    if(*reg == 0x80)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    *reg -= 1;
    *F |= SET_N;
    setSign8Bit(*reg);
    setZero(*reg);
}

void DAA(){
    // explanation at:
    //      http://z80-heaven.wikidot.com/instructions-set:daa
    uint8_t correction = 0;
    if((*A & 0X0F) > 0x09 || (*F & SET_H))
        correction += 0x06;

    if((*A > 0x99) || (*F & SET_C)){
        correction += 0x60;
        *F |= SET_C;
    }

    if(*F & SET_N){
        bool old_half = (*F & SET_H);
        bool new_half = old_half && (*A & 0x0F) < 0x06;
        if(new_half)
            *F |= SET_H;
        else
            *F &= CLEAR_H;
        *A -= correction;
    } else {
        bool new_half = (*A & 0x0F) > 0x09;
        if(new_half)
            *F |= SET_H;
        else
            *F &= CLEAR_H;
        *A += correction;
    }

    setSign8Bit(*A);
    setZero(*A);
    setParity(*A);
}

void CPL(){
    *A = ~(*A);
    *F |= SET_N;
    *F |= SET_H;
}

void SCF(){
    *F |= SET_C;
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void CCF(){
    bool carry = *F & SET_C;
    if(carry){
        *F &= CLEAR_C;
        *F |= SET_H;
    } else {
        *F |= SET_C;
        *F &= CLEAR_H;
    }
    *F &= CLEAR_N;
}

void HLT(){
}

void RETNZ(){
    if(!(*F & SET_Z))
        POP(PC);
}

void RETZ(){
    if(*F & SET_Z)
        POP(PC);
}

void RETNC(){
    if(!(*F & SET_C))
        POP(PC);
}

void RETC(){
    if(*F & SET_C)
        POP(PC);
}

void RETPO(){
    if(!(*F & SET_P))
        POP(PC);
}

void RETPE(){
    if(*F & SET_P)
        POP(PC);
}

void RETP(){
    if(!(*F & SET_S))
        POP(PC);
}

void RETM(){
    if(*F & SET_S)
        POP(PC);
}

void POP(uint16_t* reg){
    *reg = *(uint16_t*)(MEMORY+*SP);
    *SP = *SP + 2; 
}

void RET(){
    POP(PC);
}

void EXX(){
    EX(BC, BC_);
    EX(DE, DE_);
    EX(HL, HL_);
}

void JP(uint16_t val){
    *PC = val;
}

void JPNZ(uint16_t val){
    if(!(*F & SET_Z))
        JP(val);
}

void JPZ(uint16_t val){
    if(*F & SET_Z)
        JP(val);
}

void JPNC(uint16_t val){
    if(!(*F & SET_C))
        JP(val);
}

void JPC(uint16_t val){
    if(*F & SET_C)
        JP(val);
}

void JPPO(uint16_t val){
    if(!(*F & SET_P))
        JP(val);
}

void JPPE(uint16_t val){
    if(*F & SET_P)
        JP(val);
}

void JPP(uint16_t val){
    if(!(*F & SET_S))
        JP(val);
}

void JPM(uint16_t val){
    if(*F & SET_S)
        JP(val);
}

void OUT(uint8_t* io, uint8_t val){
    *io = val;
}

void IN(uint8_t* reg, uint8_t val){
    *reg = val;
}

void DI(){
    INTERRUPT_ENABLED = false;
}

void EI(){
    INTERRUPT_ENABLED = true;
}

void CALL(uint16_t val){
    PUSH(*PC);
    *PC = val;
}

void CALLNZ(uint16_t val){
    if(!(*F & SET_Z))
        CALL(val);
}

void CALLZ(uint16_t val){
    if(*F & SET_Z)
        CALL(val);
}

void CALLNC(uint16_t val){
    if(!(*F & SET_C))
        CALL(val);
}

void CALLC(uint16_t val){
    if(*F & SET_C)
        CALL(val);
}

void CALLPO(uint16_t val){
    if(!(*F & SET_P))
        CALL(val);
}

void CALLPE(uint16_t val){
    if(*F & SET_P)
        CALL(val);
}

void CALLP(uint16_t val){
    if(!(*F & SET_S))
        CALL(val);
}

void CALLM(uint16_t val){
    if(*F & SET_S)
        CALL(val);
}

void PUSH(uint16_t val){
    *SP = *SP - 2;
    *(uint16_t*)(MEMORY+*SP) = val;
}

void RST(uint8_t addr){
    CALL(addr*8);
}

void RLCA(){
    bool carry = *A & 0b10000000;
    if(carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *A = (*A << 1) | (carry);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RRCA(){
    bool carry = *A & 0b1;
    if(carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *A = (*A >> 1) | (carry << 7);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RLA(){
    bool carry = *F & SET_C;
    if(*A & 0b10000000)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *A = (*A << 1) | carry;
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RRA(){
    bool carry = *F & SET_C;
    if(*A & 0b1)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *A = (*A >> 1) | (carry << 7);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RLC(uint8_t* reg){
    bool msb = *reg >> 7;
    if(msb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *reg = (*reg << 1) | (msb);
    
    setSign8Bit(*reg);
    setZero(*reg);
    setParity(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RRC(uint8_t* reg){
    bool lsb = *reg & 0b1;
    if(lsb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *reg = (lsb << 7) | (*reg >> 1);

    setSign8Bit(*reg);
    setZero(*reg);
    setParity(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RL(uint8_t* reg){
    bool carry = *F & SET_C;
    bool msb = *reg >> 7;
    *reg = (*reg << 1) | carry;
    if(msb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    setSign8Bit(*reg);
    setZero(*reg);
    setParity(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void RR(uint8_t* reg){
    bool carry = *F & SET_C;
    bool lsb = *reg & 0b1;
    *reg = (*reg >> 1) | (carry << 7);
    if(lsb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    setSign8Bit(*reg);
    setZero(*reg);
    setParity(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void SLA(uint8_t* reg){
    bool msb = *reg >> 7;
    if(msb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *reg = *reg << 1;

    setParity(*reg);
    setSign8Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void SRA(uint8_t* reg){
    bool sign = *reg >> 7;
    bool lsb = *reg & 0b1;
    if(lsb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *reg = (*reg >> 1) | (sign << 7);

    setParity(*reg);
    setSign8Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void SLL(uint8_t* reg){
    bool msb = *reg >> 7;
    if(msb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *reg = *reg << 1;
    *reg = *reg | 0b1;

    setParity(*reg);
    setSign8Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}


void SRL(uint8_t* reg){
    bool lsb = *reg & 0b1;
    if(lsb)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    *reg = *reg >> 1;
   
    setParity(*reg);
    setSign8Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_N;
}

void BIT(uint8_t bit, uint8_t* reg){
    uint8_t masked_bit = *reg & (1 << bit);
    if(masked_bit == 0){
        *F |= SET_Z;
        *F |= SET_P;
    } else {
        *F &= CLEAR_Z;
        *F &= CLEAR_P;
    }  

    if(masked_bit & 0x80)
        *F |= SET_S;
    else
        *F &= CLEAR_S;

    *F |= SET_H;
    *F &= CLEAR_N;
}

void RES(uint8_t bit, uint8_t* reg){
    *reg = *reg & (~(uint8_t)(1 << bit));
}

void SET(uint8_t bit, uint8_t* reg){
    *reg = * reg | (1 << bit);
}

void ADD(uint8_t* reg, uint8_t val){
    uint8_t res = *reg + val;
    bool carry = calculateCarry(8, *reg, val, 0);
    if(carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    bool aux_carry = calculateCarry(4, *reg, val, 0);
    if(aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;
    
    bool overflow = calculateCarry(7, *reg, val, 0) != calculateCarry(8, *reg, val, 0);
    if(overflow)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    *reg = res;
    setSign8Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_N;
}

void ADC(uint8_t* reg, uint8_t val){
    bool carry = *F & SET_C;
    uint8_t res = *reg + val + carry;

    bool new_carry = calculateCarry(8, *reg, val, carry);
    if(new_carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    bool aux_carry = calculateCarry(4, *reg, val, carry);
    if(aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;
    
    bool overflow = calculateCarry(7, *reg, val, carry) != calculateCarry(8, *reg, val, carry);
    if(overflow)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    *reg = res;
    setSign8Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_N;
}

void SUB(uint8_t* reg, uint8_t val){
    val = ~val + 1;
    bool carry = calculateCarry(8, *reg, val - 1, 1);
    if(!carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    bool aux_carry = calculateCarry(4, *reg, val - 1, 1);
    if(!aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    uint8_t res = *reg + val;

    bool overflow = calculateCarry(7, *reg, val - 1, 1) != calculateCarry(8, *reg, val - 1, 1);
    if(overflow)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    *reg = res;
    setSign8Bit(*reg);
    setZero(*reg);
    *F |= SET_N;
}

void SBC(uint8_t* reg, uint8_t val){
    val = ~val + 1;
    bool carry = *F & SET_C;
    bool new_carry = calculateCarry(8, *reg, val - 1, !carry);
    bool aux_carry = calculateCarry(4, *reg, val - 1, !carry);

    if(!new_carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    
    if(!aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    uint8_t res = *reg + val - carry;
    bool overflow = calculateCarry(7, *reg, val - 1, !carry) != calculateCarry(8, *reg, val - 1, !carry);
    if(overflow)
        *F |= SET_P;
    else
        *F &= CLEAR_P;   

    *reg = res;
    setSign8Bit(*reg);
    setZero(*reg);
    *F |= SET_N;
}

void AND(uint8_t* reg, uint8_t val){
    *reg &= val;
    setSign8Bit(*reg);
    setZero(*reg);
    *F |= SET_H;
    *F &= CLEAR_C;
    *F &= CLEAR_N;
    setParity(*reg);
}

void XOR(uint8_t* reg, uint8_t val){
    *reg ^= val;
    setSign8Bit(*reg);
    setZero(*reg);
    setParity(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_C;
    *F &= CLEAR_N;
}

void OR(uint8_t* reg, uint8_t val){
    *reg |= val;
    setSign8Bit(*reg);
    setZero(*reg);
    setParity(*reg);
    *F &= CLEAR_H;
    *F &= CLEAR_C;
    *F &= CLEAR_N;
}

void CP(uint8_t* reg, uint8_t val){
    uint8_t copy = *reg;
    SUB(reg, val);
    *reg = copy;
}

void ADC_16(uint16_t* reg, uint16_t val){
    bool carry = *F & SET_C;

    bool new_carry = calculateCarry(16, *reg, val, carry);
    if(new_carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    
    bool aux_carry = calculateCarry(12, *reg, val, carry);
    if(aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    uint16_t res = *reg + val + carry;
    bool overflow = calculateCarry(16, *reg, val, carry) != calculateCarry(15, *reg, val, carry);
    if(overflow)
        *F |= SET_P;
    else
        *F &= CLEAR_P;
           
    *reg = res;
    setSign16Bit(*reg);
    setZero(*reg);
    *F &= CLEAR_N;
}

void SBC_16(uint16_t* reg, uint16_t val){
    val = ~val + 1;
    bool carry = *F & SET_C;
    bool new_carry = calculateCarry(16, *reg, val - 1, !carry);
    bool aux_carry = calculateCarry(12, *reg, val - 1, !carry);

    if(!new_carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;
    
    if(!aux_carry)
        *F |= SET_H;
    else
        *F &= CLEAR_H;

    uint16_t res = *reg + val - carry;
    bool overflow = calculateCarry(16, *reg, val - 1, !carry) != calculateCarry(15, *reg, val - 1, !carry);
    if(overflow)
        *F |= SET_P;
    else
        *F &= CLEAR_P;   

    *reg = res;
    setSign16Bit(*reg);
    setZero(*reg);
    *F |= SET_N;
}

void NEG(uint8_t* reg){
    uint8_t tmp = *reg;
    *reg = 0;
    SUB(reg, tmp);
}

void RETI(){
    RET();
}

void RETN(){
    RET();
}

void IM(){
    // this opcode is not used
}

void RRD(){
    uint8_t tmpA = *A;
    uint8_t val = *getReadAddress(*HL);
    *A = (tmpA & 0xF0) | (val & 0xF);
    *getWriteAddress(*HL) = (val >> 4) | (tmpA << 4);
    setSign8Bit(*A);
    setZero(*A);
    setParity(*A);
    *F &= CLEAR_N;
    *F &= CLEAR_H;
}

void RLD(){
    uint8_t tmpA = *A;
    uint8_t val = *getReadAddress(*HL);
    *A = (tmpA & 0xF0) | (val >> 4);
    *getWriteAddress(*HL) = (val << 4) | (tmpA & 0xF);
    setSign8Bit(*A);
    setZero(*A);
    setParity(*A);
    *F &= CLEAR_N;
    *F &= CLEAR_H;
}

// block instructions

void LDI(){
    *getWriteAddress(*DE) = *getReadAddress(*HL);
    *DE += 1;
    *HL += 1;
    *BC -= 1;

    if(*BC != 0)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    *F &= CLEAR_H;
    *F &= CLEAR_N;

    cpuCycles = 16;
}

void LDD(){
    *getWriteAddress(*DE) = *getReadAddress(*HL);
    *DE -= 1;
    *HL -= 1;
    *BC -= 1;

    if(*BC != 0)
        *F |= SET_P;
    else
        *F &= CLEAR_P;
    
    *F &= CLEAR_H;
    *F &= CLEAR_N;

    cpuCycles = 16;
}

void LDIR(){
    LDI();
    if(*BC != 0){
        *PC -= 2;
        *F |= SET_P;
        cpuCycles = 21;
    } else {
        *F &= CLEAR_P;
        cpuCycles = 16;
    }
}

void LDDR(){
    LDD();
    if(*BC != 0){
        *PC -= 2;
        cpuCycles = 21;
    } else {
        cpuCycles = 16;
    }
}

void CPI(){
    bool carry = (bool)(*F & SET_C);
    CP(A, *getReadAddress(*HL));
    *HL += 1;
    *BC -= 1;

    if(*BC != 0)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    if(carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    *F |= SET_N;
    cpuCycles = 16;
}

void CPD(){
    bool carry = (bool)(*F & SET_C);
    CP(A, *getReadAddress(*HL));
    *HL -= 1;
    *BC -= 1;

    if(*BC != 0)
        *F |= SET_P;
    else
        *F &= CLEAR_P;

    if(carry)
        *F |= SET_C;
    else
        *F &= CLEAR_C;

    *F |= SET_N;
    cpuCycles = 16;
}

void CPIR(){
    CPI();
    if(*BC != 0 && !(bool)(*F & SET_Z)){
        *PC -= 2;
        cpuCycles = 21;
    } else {
        cpuCycles = 16;
    }
}

void CPDR(){
    CPD();
    if(*BC != 0 && !(bool)(*F & SET_Z)){
        *PC -= 2;
        cpuCycles = 21;
    } else {
        cpuCycles = 16;
    }
}

void INI(){
    IN(getWriteAddress(*HL), IO[*C]); 
    *HL += 1;
    *B -= 1;  
}

void IND(){
    IN(getWriteAddress(*HL), IO[*C]); 
    *HL -= 1;
    *B -= 1;  
}

void INIR(){
    INI();
    if(*B != 0)
        *PC -= 2;
}

void INDR(){
    IND();
    if(*B != 0)
        *PC -= 2;
}

void OUTI(){
    OUT(IO+*C, *getReadAddress(*HL));
    *HL += 1;
    *B -= 1;
}

void OUTD(){
    OUT(IO+*C, *getReadAddress(*HL));
    *HL -= 1;
    *B -= 1;
}

void OTIR(){
    OUTI();
    if(*B != 0)
        *PC -= 2;
}

void OTDR(){
    OUTD();
    if(*B != 0)
        *PC -= 2;
}