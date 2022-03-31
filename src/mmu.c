#include "mmu.h"

mmu_t mmu;

void mmu_init()
{
    mmu.boot_rom_mapped = true;

    // Load bootrom
    memcpy(mmu.boot_rom, boot_rom, 0x100);
}

void mmu_load(uint8_t *data, uint16_t size)
{
    memcpy(mmu.rom, data, size);
}

void mmu_wb(uint16_t addr, uint8_t data)
{
    #ifdef MMU_DEBUG
    DEBUG_MMU("wb addr: %04X value: %02X\n", addr, data);
    #endif

    if (addr <= 0x7FFF) {
        // ROM (readonly)
        #ifdef MMU_DEBUG
        DEBUG_MMU("[mmu] Illegal write operation inside ROM area (%x:%x)\n", addr, data);
        #endif
    } else if (addr >= 0x8000 && addr <= 0x9FFF) {
        // VRAM
        mmu.vram[addr - 0x8000] = data;
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        // SRAM (From cartridge)
        mmu.sram[addr - 0xA000] = data;
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
        if (addr == 0xFF00) {
            // Joypad
            input_write(data);            
        } else if (addr == 0xFF01) {
            // Serial transfer data
            if (mmu.serial_control.value == 0x81) {
                #ifdef MMU_DEBUG
                DEBUG_MMU("Serial Transfer -> %c\n", data);
                #endif

                mmu.serial_control.fields.start_flag = 0;
            }
        } else if (addr == 0xFF02) {
            mmu.serial_control.value = data;  
        } else if (addr == 0xFF0F) {
            // Interrupt flags
            cpu.ifr = data;
        } else if (addr >= 0xFF40 && addr <= 0xFF4B) {
            lcd_wb(addr & 0xFF, data);
        } else if (addr == 0xFF50) {
            mmu.boot_rom_mapped = false;

            #ifdef MMU_DEBUG
            DEBUG_MMU("Unmapped boot rom\n");
            #endif
        }

    } else if (addr >= 0xFF80 && addr <= 0xFFFE) {
        // HRAM
        mmu.hram[addr - 0xFF80] = data;
    } else if (addr == 0xFFFF) {
        cpu_enable_interrupts(data);
    } else {
        #ifdef MMU_DEBUG
        DEBUG_MMU("[mmu] Illegal write operation (%x:%x)\n", addr, data);
        #endif
    }
}

void mmu_ww(uint16_t addr, uint16_t data)
{
    mmu_wb(addr, data & 0xFF);
    mmu_wb(addr + 1, (data >> 8) & 0xFF);
}

uint8_t mmu_rb(uint16_t addr)
{
    uint8_t result;

    if (addr <= 0x7FFF) {
        // Check if boot rom is still mapped
        if (addr <= 0x00FF && mmu.boot_rom_mapped) {
            // Boot ROM
            result =  mmu.boot_rom[addr];
        } else {
            // ROM
            result = mmu.rom[addr];

            #ifdef MMU_DEBUG
            if (mmu.boot_rom_mapped) {
                DEBUG_MMU("Read operation from rom during mapped boot rom");
            }
            #endif
        }

    } else if (addr >= 0x8000 && addr <= 0x9FFF) {
        // VRAM
        result = mmu.vram[addr - 0x8000];
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        // SRAM (From cartridge)
        result = mmu.sram[addr - 0xA000];
    } else if (addr >= 0xC000 && addr <= 0xDFFF) {
        // WRAM
        result = mmu.wram[addr - 0xC000];
    } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
        // OAM
        result = mmu.oam[addr - 0xFE00];
    } else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
        result = 0;
    } else if (addr >= 0xFF00 && addr <= 0xFF7F) {
        // IO
        if (addr == 0xFF00) {
            result = input_read();
        } else if (addr == 0xFF01) {
            result = 0;
        } else if (addr == 0xFF02) {
            result = mmu.serial_control.value;
        } else if (addr == 0xFF04) {
            // Timer divider
            result = (uint8_t) rand();
        } else if (addr == 0xFF0F) {
            // Interrupt flag
            result = cpu.ifr;
        } else if (addr >= 0xFF40 && addr <= 0xFF4B) {
            result = lcd_rb(addr & 0xFF);
        }
    } else if (addr >= 0xFF80 && addr <= 0xFFFE) {
        result = mmu.hram[addr - 0xFF80];
    } else if (addr == 0xFFFF) {
        result = cpu.ie;
    } else {
        #ifdef MMU_DEBUG
        DEBUG_MMU("[mmu] Illegal read operation (%x)\n", addr);
        #endif
        result = 0;
    }

    #ifdef MMU_DEBUG
    DEBUG_MMU("rb addr: %04X value: %02X\n", addr, result);
    #endif

    return result;
}

uint16_t mmu_rw(uint16_t addr)
{
    return mmu_rb(addr) | (mmu_rb(addr + 1) << 8);
}