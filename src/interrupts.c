#include "emulator.h"

void interrupts_step()
{
    char *interrupt_name;
    if (cpu.ime && cpu.ifr && cpu.ie) {
        uint8_t triggered = cpu.ie & cpu.ifr;

        /* TODO: Wake up cpu even if ime is not set (if halted) */

        if (triggered) {
            cpu.regs.sp -= 2;
            mmu_ww(cpu.regs.sp, cpu.regs.pc);
        }

        printf("triggered: %x\n", triggered);

        if (triggered & CPU_IF_VBLANK) {
            cpu.regs.pc = 0x0040;
            interrupt_name = "VBLANK";
            cpu.ifr &= ~CPU_IF_VBLANK;
        } else if (triggered & CPU_IF_LCD_STAT) {
            cpu.regs.pc = 0x0048;
            interrupt_name = "STAT";
            cpu.ifr &= ~CPU_IF_LCD_STAT;
        } else if (triggered & CPU_IF_TIMER) {
            cpu.regs.pc = 0x0050;
            interrupt_name = "TIMER";
            cpu.ifr &= ~CPU_IF_TIMER;
        } else if (triggered & CPU_IF_SERIAL) {
            cpu.regs.pc = 0x0058;
            interrupt_name = "SERIAL";
            cpu.ifr &= ~CPU_IF_SERIAL;
        } else if (triggered & CPU_IF_JOYPAD) {
            cpu.regs.pc = 0x0060;
            interrupt_name = "JOYPAD";
            cpu.ifr &= ~CPU_IF_SERIAL;
        }

        cpu.ime = false;
        cpu.halted = false;

        printf("ifr: %x\n", cpu.ifr);

        #ifdef CPU_DEBUG
        DEBUG_CPU("Interrupt %s after %d cycles\n", interrupt_name, cpu.cycles);
        #endif
    }
}