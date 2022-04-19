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
    uint8_t result = 0x3F;

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

    #ifdef INPUT_DEBUG
    DEBUG_INPUT("Read: %x\n", result);

    if (input.direction) {
        DEBUG_INPUT(" Right:  %s\n", input.state.right ? "Pressed" : "Released");
        DEBUG_INPUT(" Left:   %s\n", input.state.left ? "Pressed" : "Released");
        DEBUG_INPUT(" Up:     %s\n", input.state.up ? "Pressed" : "Released");
        DEBUG_INPUT(" Down:   %s\n", input.state.down ? "Pressed" : "Released");
    }

    if (input.action) {
        DEBUG_INPUT(" A:      %s\n", input.state.a ? "Pressed" : "Released");
        DEBUG_INPUT(" B:      %s\n", input.state.b ? "Pressed" : "Released");
        DEBUG_INPUT(" Select: %s\n", input.state.select ? "Pressed" : "Released");
        DEBUG_INPUT(" Start:  %s\n", input.state.start ? "Pressed" : "Released");
    }

    #endif

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

    if (event->repeat) {
        return;
    }

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

        case SDL_SCANCODE_LSHIFT:
            input.state.select = !release;
            break;

        case SDL_SCANCODE_RETURN:
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

    //cpu_request_interrupt(CPU_IF_JOYPAD);
}