#include "emulator.h"

timer_t timer;

#ifdef TIMER_DEBUG
#define DEBUG_TIMER(...) printf("[timer] "); printf(__VA_ARGS__)
#endif


void timer_tima_increment()
{
    timer.tima++;

    #ifdef TIMER_DEBUG
    DEBUG_TIMER("TIMA increment\n");
    #endif

    if (timer.tima == 0) {
        timer.tima = timer.tma;
        cpu_request_interrupt(CPU_IF_TIMER);

        #ifdef TIMER_DEBUG
        DEBUG_TIMER("TIMA overflow\n");
        #endif
    }
}

void timer_tick(uint32_t cycles)
{
    timer.cycles += cycles / TIMER_BASE_CLOCK;

    if ((timer.cycles % 16) == 0) {
        timer.div++;

        /*
        #ifdef TIMER_DEBUG
        DEBUG_TIMER("DIV increment\n");
        #endif
        */
    }

    if (timer.tac & TIMER_TAC_ENABLE) {
        uint8_t clock_select = timer.tac & 3;
        int divider;

        switch(clock_select) {
            case TIMER_TAC_CLOCK_1024:
                divider = 1024;
                break;
            
            case TIMER_TAC_CLOCK_16:
                divider = 16;
                break;

            case TIMER_TAC_CLOCK_64:
                divider = 64;
                break;

            case TIMER_TAC_CLOCK_256:
                divider = 256;
                break;
        }

        if ((timer.cycles % divider) == 0) {
            timer_tima_increment();
        }
    }
}