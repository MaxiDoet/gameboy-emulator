#include "cpu.h"

Cpu::Cpu()
{
    this->reset();
}

void Cpu::step()
{
    
}

void Cpu::reset()
{
    this->regs.af = 0x0000;
    this->regs.bc = 0x0000;
    this->regs.de = 0x0000;
    this->regs.hl = 0x0000;
    this->regs.sp = 0x0000;
    this->regs.pc = 0x0000;
}