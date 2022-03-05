#ifndef _cpu_h
#define _cpu_h

#include <cstdint>

struct cpu_regs {
    union {
        struct {
            uint8_t a;
            uint8_t f;
        };

        uint16_t af;
    };

    union {
        struct {
            uint8_t b;
            uint8_t c;
        };

        uint16_t bc;
    };
    
    union {
        struct {
            uint8_t d;
            uint8_t e;
        };

        uint16_t de;
    };

    union {
        struct {
            uint8_t h;
            uint8_t l;
        };

        uint16_t hl;
    };

    uint16_t sp;
    uint16_t pc;
};

class Cpu {
public:
    Cpu();

    void step();
    void reset();

private:
    cpu_regs regs;
};

#endif