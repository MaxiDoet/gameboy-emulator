#ifndef _emulator_h
#define _emulator_h

#include "cpu.h"
#include "mmu.h"

class Emulator {
public:
    Emulator();

    void run();
    int load(const char *path);

private:
    Cpu cpu;
    MMU mmu;
};

#endif