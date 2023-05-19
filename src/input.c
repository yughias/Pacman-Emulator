#include <hardware.h>
#include <SDL_MAINLOOP.h>

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
}