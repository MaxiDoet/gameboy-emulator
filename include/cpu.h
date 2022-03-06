#ifndef _cpu_h
#define _cpu_h

#include <stdint.h>

#define CPU_DEBUG

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
    bool ime;
    bool halt;
    bool stop;
} cpu_t;

void cpu_init();
void cpu_reset();
void cpu_step();

extern cpu_t cpu;

#endif