#ifndef _timer_h
#define _timer_h

#include "emulator.h"

#define TIMER_DEBUG

#define TIMER_BASE_CLOCK        262144
#define TIMER_TAC_ENABLE        (1 << 2)
#define TIMER_TAC_CLOCK_1024    0
#define TIMER_TAC_CLOCK_16      1
#define TIMER_TAC_CLOCK_64      2
#define TIMER_TAC_CLOCK_256     3

typedef struct timer_t {
    /* Registers */
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    uint32_t cycles;
} timer_t;

extern timer_t timer;

void timer_tick(uint32_t cycles);

#endif