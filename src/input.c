#include "emulator.h"

#ifdef INPUT_DEBUG
#define DEBUG_INPUT(...) printf("[input] "); printf(__VA_ARGS__)
#endif

input_state_t input_state;

void input_init()
{
    input_state.value = 0xFF;
}

uint8_t input_read()
{
    return input_state.value;
}

void input_write(uint8_t value)
{
    // Bits 0-3 are read only
    input_state.value = (value & 0x30);

    #ifdef INPUT_DEBUG
    DEBUG_INPUT("-> JOYP: %x\n", value);

    DEBUG_INPUT("Selected:\n");
    DEBUG_INPUT(" %c Action\n", input_state.fields.select_action ? '*' : ' ');
    DEBUG_INPUT(" %c Direction\n", input_state.fields.select_direction ? '*' : ' ');
    #endif
}