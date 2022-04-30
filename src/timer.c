#include "emulator.h"

timer_regs_t timer;

#ifdef TIMER_DEBUG
#define DEBUG_TIMER(...) printf("[timer] "); printf(__VA_ARGS__)
#endif

void timer_wb(uint8_t addr, uint8_t data)
{
    switch(addr) {
        case 0x04:
            timer.div = 0;

            #ifdef TIMER_DEBUG
            DEBUG_TIMER("-> DIV: Reset\n");
            #endif
            break;

        case 0x05:
            timer.tima = data;

            #ifdef TIMER_DEBUG
            DEBUG_TIMER("-> TIMA: %02X\n", data);
            #endif
            break;

        case 0x06:
            timer.tma = data;

            #ifdef TIMER_DEBUG
            DEBUG_TIMER("-> TMA: %02X\n", data);
            #endif
            break;

        case 0x07:
            timer.tac = data;

            #ifdef TIMER_DEBUG

            uint16_t divider = 0;

            switch(timer.tac & 3) {
                case 0:
                    timer.freq = 4096;
                    break;
            
                case 1:
                    timer.freq = 262144;
                    break;

                case 2:
                    timer.freq = 65536;
                    break;

                case 3:
                    timer.freq = 16384;
                    break;
            }

            DEBUG_TIMER("-> TMA: %02X | Frequency: %d Timer enable: %s\n",
                        data,
                        timer.freq,
                        ((timer.tac & (1 << 2))) ? "Enabled" : "Disabled"
                        );
            #endif
            break;
    }
}

uint8_t timer_rb(uint8_t addr)
{
    uint8_t result = 0;

    switch (addr) {
        case 0x04:
            result = timer.div;
            break;

        case 0x05:
            result = timer.tima;
            break;

        case 0x06:
            result = timer.tma;
            break;

        case 0x07:
            result = timer.tac;
            break;
    }

    return result;
}

void timer_tick(uint32_t cycles)
{
    // DIV
    timer.div_counter += cycles;
    
    if (timer.div_counter >= 256) {
        timer.div_counter -= 256;
        timer.div++;

        #ifdef TIMER_DEBUG
        DEBUG_TIMER("DIV increment\n");
        #endif
    }

    // TIMA
    if (timer.tac & TIMER_TAC_ENABLE) {
        timer.tima_counter += cycles;

        if ((timer.tima_counter % (4194304 / timer.freq)) == 0) {
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