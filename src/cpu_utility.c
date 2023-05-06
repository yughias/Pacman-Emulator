#include <cpu_utility.h>

void setParity(uint8_t val){
    int counter = 0;
    while(val != 0){
        counter += (val & 0x1);
        val = val >> 1;
    }
    bool parity = (counter % 2 == 0);
    if(parity)
        *F |= SET_P;
    else
        *F &= CLEAR_P;
}

void setZero(uint16_t val){
    if(val == 0)
        *F |= SET_Z;
    else
        *F &= CLEAR_Z;
}

void setSign8Bit(uint8_t val){
    if(val & 0x80)
        *F |= SET_S;
    else
        *F &= CLEAR_S;
}

void setSign16Bit(uint16_t val){
    if(val & 0x8000)
        *F |= SET_S;
    else
        *F &= CLEAR_S;
}

bool calculateCarry(int bit, uint16_t a, uint16_t b, bool cy) {
  int32_t result = a + b + cy;
  int32_t carry = result ^ a ^ b;
  return carry & (1 << bit);
}