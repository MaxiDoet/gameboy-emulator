#ifndef _input_h
#define _input_h

//#define INPUT_DEBUG

typedef struct input_state_t {
    uint8_t a           : 1;
    uint8_t b           : 1;
    uint8_t select      : 1;
    uint8_t start       : 1;
    uint8_t up          : 1;
    uint8_t down        : 1;
    uint8_t left        : 1;
    uint8_t right       : 1;
} input_state_t;

typedef struct input_t {
    input_state_t state;
    bool direction;
    bool action;
} input_t;

extern input_t input;

void input_init();
uint8_t input_read();
void input_write(uint8_t value);
void input_handle(SDL_KeyboardEvent *event);

#endif