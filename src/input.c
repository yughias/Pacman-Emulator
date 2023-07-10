#include <hardware.h>
#include <SDL2/SDL.h>

const char coins_per_game_descriptor[4][32] = {
    "free play",
    "1 coin per game",
    "1 coin per 2 games",
    "2 coins per game"
};

const char lives_per_game_descriptor[4][32] = {
    "1 life",
    "2 lives",
    "3 lives",
    "5 lives"
};

const char bonus_score_descriptor[4][32] = {
    "10000 points",
    "15000 points",
    "20000 points",
    "no bonus"
};

const char difficulty_descriptor[2][32] = {
    "hard mode",
    "normal mode"
};

const char ghost_names_descriptor[2][32] = {
    "alternate ghost names",
    "normal ghost names"
};

void updateInput(const Uint8* keyState){
    IN0 = 0xFF;
    if(keyState[SDL_SCANCODE_UP])
        IN0 &= ~(uint8_t)(1 << 0);
    if(keyState[SDL_SCANCODE_LEFT])
        IN0 &= ~(uint8_t)(1 << 1);
    if(keyState[SDL_SCANCODE_RIGHT])
        IN0 &= ~(uint8_t)(1 << 2);
    if(keyState[SDL_SCANCODE_DOWN])
        IN0 &= ~(uint8_t)(1 << 3);
    if(keyState[SDL_SCANCODE_RSHIFT])
        IN0 &= ~(uint8_t)(1 << 7);

    IN1 = 0xFF;
    if(keyState[SDL_SCANCODE_1])
        IN1 &= ~(uint8_t)(1 << 5);
    if(keyState[SDL_SCANCODE_2])
        IN1 &= ~(uint8_t)(1 << 6);

    if(isKeyReleased){
        uint8_t info;
        switch (keyReleased){
            case SDLK_6:
            info = increaseDipSwitch(MASK_COINS_PER_GAME, N_BITS_COINS_PER_GAME, SHIFT_COINS_PER_GAME);
            printf("%s\n", coins_per_game_descriptor[info]);
            break;

            case SDLK_7:
            info = increaseDipSwitch(MASK_LIVES_PER_GAME, N_BITS_LIVES_PER_GAME, SHIFT_LIVES_PER_GAME);
            printf("%s\n", lives_per_game_descriptor[info]);
            break;

            case SDLK_8:
            info = increaseDipSwitch(MASK_BONUS_SCORE, N_BITS_BONUS_SCORE, SHIFT_BONUS_SCORE);
            printf("%s\n", bonus_score_descriptor[info]);
            break;

            case SDLK_9:
            info = increaseDipSwitch(MASK_DIFFICULTY, N_BITS_DIFFICULTY, SHIFT_DIFFICULTY);
            printf("%s\n", difficulty_descriptor[info]);
            break;

            case SDLK_0:
            info = increaseDipSwitch(MASK_GHOST_NAMES, N_BITS_GHOST_NAMES, SHIFT_GHOST_NAMES);
            printf("%s\n", ghost_names_descriptor[info]);
            break;
        }
    }
}

uint8_t increaseDipSwitch(uint8_t dip_mask, uint8_t n_bits, uint8_t n_shift){
    uint8_t setting = DIP_SWITCH_SETTINGS & dip_mask;
    setting >>= n_shift; 
    setting = (setting + 1) & ((1 << n_bits) - 1);
    DIP_SWITCH_SETTINGS = (DIP_SWITCH_SETTINGS & ~(dip_mask)) | (setting << n_shift);
    return setting;
}