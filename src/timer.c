#include "emulator.h"

timer_regs_t timer;

#ifdef TIMER_DEBUG
#define DEBUG_TIMER(...) printf("[timer] "); printf(__VA_ARGS__)
#endif

void timer_tick(uint32_t cycles)
{
    // DIV
    timer.div_counter += cycles;
    
    //printf("cycles: %d div_counter: %d\n", cycles, timer.div_counter);

    if (timer.div_counter >= 256) {
        timer.div_counter = 0;
        timer.div++;

        #ifdef TIMER_DEBUG
        DEBUG_TIMER("DIV increment\n");
        #endif
    }

    // TIMA
    if (timer.tac & TIMER_TAC_ENABLE) {
        timer.tima_counter += cycles;

        uint8_t clock_select = timer.tac & 3;
        
        uint16_t divider = 0;

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

        if (timer.tima_counter >= divider) {
            if (timer.tima == 0xFF) {
                timer.tima = timer.tma;

                cpu_request_interrupt(CPU_IF_TIMER);

                #ifdef TIMER_DEBUG
                DEBUG_TIMER("TIMA overflow\n");
                #endif
            } else {
                timer.tima += 1;
            }
        }
    }
}