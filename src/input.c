#include "emulator.h"

#ifdef INPUT_DEBUG
#define DEBUG_INPUT(...) printf("[input] "); printf(__VA_ARGS__)
#endif

input_t input;

void input_init()
{
    
}

uint8_t input_read()
{
    uint8_t result = 0xFF;

    if (input.direction) {
        if (input.state.right) result &= ~(1 << 0);
        if (input.state.left) result &= ~(1 << 1);
        if (input.state.up) result &= ~(1 << 2);
        if (input.state.down) result &= ~(1 << 3);
    }

    if (input.action) {
        if (input.state.a) result &= ~(1 << 0);
        if (input.state.b) result &= ~(1 << 1);
        if (input.state.select) result &= ~(1 << 2);
        if (input.state.start) result &= ~(1 << 3);
    }

    if (input.direction) result &= ~(1 << 4);
    if (input.action) result &= ~(1 << 5);

    return result;
}

void input_write(uint8_t value)
{
    input.direction = !(value & (1 << 4));
    input.action = !(value & (1 << 5));

    #ifdef INPUT_DEBUG
    DEBUG_INPUT("-> JOYP: %x\n", value);

    DEBUG_INPUT("Selected:\n");
    DEBUG_INPUT(" %c Action\n", input.action ? '*' : ' ');
    DEBUG_INPUT(" %c Direction\n", input.direction ? '*' : ' ');
    #endif
}

void input_handle(SDL_KeyboardEvent *event)
{
    bool release = (event->type == SDL_KEYUP);

    #ifdef INPUT_DEBUG
    DEBUG_INPUT("Event Key: %s State: %s\n", SDL_GetKeyName(event->keysym.sym), release ? "Release" : "Press");
    #endif

    switch(event->keysym.scancode) {
        case SDL_SCANCODE_A:
            input.state.a = !release;
            break;
        
        case SDL_SCANCODE_B:
            input.state.b = !release;
            break;

        case SDL_SCANCODE_X:
            input.state.select = !release;
            break;

        case SDL_SCANCODE_Y:
            input.state.start = !release;
            break;

        case SDL_SCANCODE_RIGHT:
            input.state.right = !release;
            break;

        case SDL_SCANCODE_LEFT:
            input.state.left = !release;
            break;

        case SDL_SCANCODE_UP:
            input.state.up = !release;
            break;

        case SDL_SCANCODE_DOWN:
            input.state.down = !release;
            break;

        default:
            break;            
    }
}