#ifndef __INPUT_H__
#define __INPUT_H__

#include <SDL2/SDL.h>

#define MASK_COINS_PER_GAME    0b11
#define MASK_LIVES_PER_GAME    0b1100
#define MASK_BONUS_SCORE       0b110000
#define MASK_DIFFICULTY        0b1000000
#define MASK_GHOST_NAMES       0b10000000

#define N_BITS_COINS_PER_GAME  2
#define N_BITS_LIVES_PER_GAME  2
#define N_BITS_BONUS_SCORE     2
#define N_BITS_DIFFICULTY      1
#define N_BITS_GHOST_NAMES     1

#define SHIFT_COINS_PER_GAME   0
#define SHIFT_LIVES_PER_GAME   2
#define SHIFT_BONUS_SCORE      4
#define SHIFT_DIFFICULTY       6
#define SHIFT_GHOST_NAMES      7

void updateInput(const Uint8*);
uint8_t increaseDipSwitch(uint8_t, uint8_t, uint8_t);

#endif