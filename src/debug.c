#include "emulator.h"

void debug_mem_dump(uint16_t start, uint16_t size)
{
    printf("[debug] memdump start: %04X size: %04X\n", start, size);

    for (int i=0; i < (size / 8); i++) {
        for (int j=0; j < 8; j++) {
            printf("%x ", mmu_rb(start + i*8 + j));
        }

        printf("\n");
    }
}

void debug_reg_dump()
{
    printf("[debug] regdump: A: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X | F: %02X PC: %04X SP: %04X IE: %02X IF: %02X Cycles: %d\n",
                cpu.regs.a,
                cpu.regs.b,
                cpu.regs.c,
                cpu.regs.d,
                cpu.regs.e,
                cpu.regs.h,
                cpu.regs.l,
                cpu.regs.f,
                cpu.regs.pc - 1,
                cpu.regs.sp,
                cpu.ie,
                cpu.ifr,
                cpu.cycles
            );
}