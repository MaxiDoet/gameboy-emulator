#ifndef _timer_h
#define _timer_h

#include "emulator.h"

#define TIMER_DEBUG

#define TIMER_TAC_ENABLE        (1 << 2)
#define TIMER_CYCLES_PER_DIV    16384

typedef struct timer_regs_t {
    /* Registers */
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    uint32_t freq;
    uint32_t div_counter;
    uint32_t tima_counter;
} timer_regs_t;

extern timer_regs_t timer;

void timer_wb(uint8_t addr, uint8_t data);
uint8_t timer_rb(uint8_t addr);
void timer_tick(uint32_t cycles);

#endif