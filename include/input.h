#ifndef _input_h
#define _input_h

#define INPUT_DEBUG

typedef union {
    struct {
        uint8_t right_a             : 1;
        uint8_t left_b              : 1;
        uint8_t up_select           : 1;
        uint8_t down_start          : 1;
        uint8_t select_direction    : 1;
        uint8_t select_action       : 1;
    } fields;

    uint8_t value;  
} input_state_t;

extern input_state_t input_state;

void input_init();
uint8_t input_read();
void input_write(uint8_t value);
//void input_press();

#endif