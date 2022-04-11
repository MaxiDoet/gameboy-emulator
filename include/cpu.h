#ifndef _cpu_h
#define _cpu_h

#include <stdint.h>

#define CPU_DEBUG
#define CPU_DEBUG_INSTRUCTIONS
#define CPU_DEBUG_INTERRUPTS

#ifdef CPU_DEBUG
#define DEBUG_CPU(...) printf("[cpu] "); printf(__VA_ARGS__); fflush(stdout);
#endif

typedef struct cpu_regs_t {
    union {
        struct {
            uint8_t f;
            uint8_t a;
        };

        uint16_t af;
    };

    union {
        struct {
            uint8_t c;
            uint8_t b;
        };

        uint16_t bc;
    };
    
    union {
        struct {
            uint8_t e;
            uint8_t d;
        };

        uint16_t de;
    };

    union {
        struct {
            uint8_t l;
            uint8_t h;
        };

        uint16_t hl;
    };

    uint16_t sp;
    uint16_t pc; 
} cpu_regs_t;

typedef struct cpu_t {
    cpu_regs_t regs;
    int32_t cycles;
    uint8_t ie;
    uint8_t ifr;

    bool ime;
    bool halted;
    bool stopped;
    bool debug_enabled;
} cpu_t;

void cpu_init();
void cpu_reset();
void cpu_serve_interrupts();
void cpu_step();
void cpu_request_interrupt(uint8_t ifr);
void cpu_enable_interrupts(uint8_t ie);

#define CPU_IE_VBLANK (1 << 0)
#define CPU_IE_LCD_STAT (1 << 1)
#define CPU_IE_TIMER (1 << 2)
#define CPU_IE_SERIAL (1 << 3)
#define CPU_IE_JOYPAD (1 << 4)

#define CPU_IF_VBLANK (1 << 0)
#define CPU_IF_LCD_STAT (1 << 1)
#define CPU_IF_TIMER (1 << 2)
#define CPU_IF_SERIAL (1 << 3)
#define CPU_IF_JOYPAD (1 << 4)

extern cpu_t cpu;

#endif