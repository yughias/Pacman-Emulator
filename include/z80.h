#ifndef _Z80_H_
#define _Z80_H_


#define HERTZ 3.072e6

// flag masks to set/clear registers
#define SET_S   0b10000000
#define SET_Z   0b01000000
#define SET_H   0b00010000
#define SET_P   0b00000100
#define SET_N   0b00000010
#define SET_C   0b00000001
#define CLEAR_S 0b01111111
#define CLEAR_Z 0b10111111
#define CLEAR_H 0b11101111
#define CLEAR_P 0b11111011
#define CLEAR_N 0b11111101
#define CLEAR_C 0b11111110

// check if cpu is working
extern bool HALTED;
extern bool INTERRUPT_ENABLED;
extern bool INTERRUPT_PENDING;
extern uint16_t INTERRUPT_VECT;

// registers in 16 bit mode
extern uint16_t AF[1];
extern uint16_t BC[1];
extern uint16_t DE[1];
extern uint16_t HL[1];

extern uint16_t AF_[1];
extern uint16_t BC_[1];
extern uint16_t DE_[1];
extern uint16_t HL_[1];

extern uint16_t IX[1];
extern uint16_t IY[1];

extern uint8_t* IXL;
extern uint8_t* IXH;
extern uint8_t* IYL;
extern uint8_t* IYH;

// registers in 8 bit mode
extern uint8_t* A;
extern uint8_t* F;
extern uint8_t* B;
extern uint8_t* C;
extern uint8_t* D;
extern uint8_t* E;
extern uint8_t* H;
extern uint8_t* L;

extern uint8_t I[1];
extern uint8_t R[1];

// other 16 bit registers
extern uint16_t SP[1];
extern uint16_t PC[1];

extern uint64_t cpuCycles;

void initCPU();
void infoCPU();
void stepCPU();
void useDDRegisterTable();
void useFDRegisterTable();
void restoreRegisterTable();
void adjustDDorFFOpcode();
void processInterrupt();

//z80 NEW INSTRUCTION
void NOP();
void EX(uint16_t*, uint16_t*);
void DJNZ(int8_t);
void JR(int8_t);
void JRNZ(int8_t);
void JRZ(int8_t);
void JRNC(int8_t);
void JRC(int8_t);
void LD_8(uint8_t*, uint8_t);
void LD_16(uint16_t*, uint16_t);
void ADD_16(uint16_t*, uint16_t*);
void INC_16(uint16_t*);
void DEC_16(uint16_t*);
void INC_8(uint8_t*);
void DEC_8(uint8_t*);
void DAA();
void CPL();
void SCF();
void CCF();
void HLT();
void RETNZ();
void RETZ();
void RETNC();
void RETC();
void RETPO();
void RETPE();
void RETP();
void RETM();
void POP(uint16_t*);
void RET();
void EXX();
void JP(uint16_t);
void JPNZ(uint16_t);
void JPZ(uint16_t);
void JPNC(uint16_t);
void JPC(uint16_t);
void JPPO(uint16_t);
void JPPE(uint16_t);
void JPP(uint16_t);
void JPM(uint16_t);
void OPCODE_OUT(uint8_t*, uint8_t);
void OPCODE_IN(uint8_t*, uint8_t);
void EI();
void DI();
void CALL(uint16_t);
void CALLNZ(uint16_t);
void CALLZ(uint16_t);
void CALLNC(uint16_t);
void CALLC(uint16_t);
void CALLPO(uint16_t);
void CALLPE(uint16_t);
void CALLP(uint16_t);
void CALLM(uint16_t);
void PUSH(uint16_t);
void RST(uint8_t);
void RLC(uint8_t*);
void RRC(uint8_t*);
void RLCA();
void RRCA();
void RLA();
void RRA();
void RLC(uint8_t*);
void RRC(uint8_t*);
void RL(uint8_t*);
void RR(uint8_t*);
void SLA(uint8_t*);
void SRA(uint8_t*);
void SLL(uint8_t*);
void SRL(uint8_t*);
void BIT(uint8_t, uint8_t*);
void SET(uint8_t, uint8_t*);
void RES(uint8_t, uint8_t*);
void ADD(uint8_t*, uint8_t);
void ADC(uint8_t*, uint8_t);
void SUB(uint8_t*, uint8_t);
void SBC(uint8_t*, uint8_t);
void AND(uint8_t*, uint8_t);
void XOR(uint8_t*, uint8_t);
void OR(uint8_t*, uint8_t);
void CP(uint8_t*, uint8_t);
void ADC_16(uint16_t*, uint16_t);
void SBC_16(uint16_t*, uint16_t);
void NEG(uint8_t*);
void RETI();
void RETN();
void IM();
void RRD();
void RLD();

// block instruction
void LDI();
void LDD();
void LDIR();
void LDDR();
void CPI();
void CPD();
void CPIR();
void CPDR();
void INI();
void IND();
void INIR();
void INDR();
void OUTI();
void OUTD();
void OTIR();
void OTDR();


#endif