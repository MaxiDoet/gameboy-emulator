#include "mmu.h"

mmu_t mmu;

void mmu_init()
{
    memset(mmu.rom, 0x00, 0x7FFF);
    memset(mmu.vram, 0x00, 0x1FFF);
    memset(mmu.wram, 0x00, 0x0FFF);
    memset(mmu.oam, 0x00, 0x009F);
    memset(mmu.hram, 0x00, 0x007E);
}

void mmu_load(uint8_t *data, uint16_t size)
{
    memcpy(mmu.rom, data, size);
}

void mmu_wb(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr <= 0x7FFF) {
        // ROM (readonly)
        printf("[mmu] Illegal write operation inside ROM area (%x:%x)\n", addr, data);
    } else if (addr >= 0x8000 && addr <= 0x9FFF) {
        // VRAM
        mmu.vram[addr - 0x8000] = data;
    } else if (addr >= 0xC000 && addr <= 0xDFFF) {
        // WRAM
        mmu.wram[addr - 0xC000] = data;
    } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
        // OAM
        mmu.oam[addr - 0xFE00] = data;
    } else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
        return;
    } else if (addr >= 0xFF00 && addr <= 0xFF7F) {
        // IO
        if (addr == 0xFF0F) {
            // Interrupt flags
            cpu.ifr = data;
        } else if (addr >= 0xFF40 && addr <= 0xFF4B) {
            lcd_wb(addr & 0xFF, data);
        }

    } else if (addr >= 0xFF80 && addr <= 0xFFFE) {
        // HRAM
        mmu.hram[addr - 0xFF80] = data;
    } else if (addr == 0xFFFF) {
        cpu_enable_interrupts(data);
    } else {
        printf("[mmu] Illegal write operation (%x:%x)\n", addr, data);
    }
}

void mmu_ww(uint16_t addr, uint16_t data)
{
    mmu_wb(addr, data & 0xFF);
    mmu_wb(addr + 1, (data >> 8) & 0xFF);
}

uint8_t mmu_rb(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x7FFF) {
        // ROM (readonly)
        return mmu.rom[addr];
    } else if (addr >= 0x8000 && addr <= 0x9FFF) {
        // VRAM
        return mmu.vram[addr - 0x8000];
    } else if (addr >= 0xC000 && addr <= 0xDFFF) {
        // WRAM
        return mmu.wram[addr - 0xC000];
    } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
        // OAM
        return mmu.oam[addr - 0xFE00];
    } else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
        return 0;
    } else if (addr >= 0xFF00 && addr <= 0xFF7F) {
        // IO
        if (addr == 0xFF0F) {
            // Interrupt flag
            return cpu.ifr;
        } else if (addr >= 0xFF40 && addr <= 0xFF4B) {
            return lcd_rb(addr & 0xFF);
        }
    } else if (addr >= 0xFF80 && addr <= 0xFFFE) {
        return mmu.hram[addr - 0xFF80];
    } else if (addr == 0xFFFF) {
        return cpu.ie;
    } else {
        printf("[mmu] Illegal read operation (%x)\n", addr);
        return 0;
    }
}

uint16_t mmu_rw(uint16_t addr)
{
    return mmu_rb(addr) | (mmu_rb(addr + 1) << 8);
}