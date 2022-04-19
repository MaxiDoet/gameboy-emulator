#ifndef _timer_h
#define _timer_h

#include "emulator.h"

//#define TIMER_DEBUG

#define TIMER_TAC_ENABLE        (1 << 2)
#define TIMER_CYCLES_PER_DIV    16384
#define TIMER_TAC_CLOCK_1024    0
#define TIMER_TAC_CLOCK_16      1
#define TIMER_TAC_CLOCK_64      2
#define TIMER_TAC_CLOCK_256     3

typedef struct timer_regs_t {
    /* Registers */
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    uint32_t div_counter;
    uint32_t tima_counter;
} timer_regs_t;

extern timer_regs_t timer;

void timer_tick(uint32_t cycles);

#endif