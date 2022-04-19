#include "mmu.h"

mmu_t mmu;

#ifdef MMU_DEBUG
#define DEBUG_MMU(...) printf("[mmu] "); printf(__VA_ARGS__)
#endif

void mmu_init()
{
    memset(mmu.rom, 0x00, 0x8000);
    memset(mmu.vram, 0x00, 0x2000);
    memset(mmu.sram, 0x00, 0x2000);
    memset(mmu.wram, 0x00, 0x8000);
    memset(mmu.oam, 0x00, 0x0100);
    memset(mmu.hram, 0x00, 0x007F);

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
    if (addr <= 0x7FFF) {
        // ROM
        if (emu.rom_info.cartridge_type != ROM_CARTRIDGE_TYPE_ROMONLY) {
            mbc_wb(addr, data);
        }

        #if defined MMU_DEBUG
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
            mmu.serial_data = data;
        } else if (addr == 0xFF02) {
            mmu.serial_control.value = data;  

            if (mmu.serial_control.fields.start_flag) {
                #ifdef MMU_DEBUG
                DEBUG_MMU("Serial Transfer -> %c\n", mmu.serial_data);
                #endif

                //cpu_request_interrupt(CPU_IF_SERIAL);
            }
        } else if (addr == 0xFF04) {
            // Timer DIV
            timer.div = 0;
        } else if (addr == 0xFF05) {
            // Timer TIMA
            timer.tima = data;
        } else if (addr == 0xFF06) {
            // Timer TMA
            timer.tma = data;
        } else if (addr == 0xFF07) {
            // Timer TAC
            timer.tac = data;
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

    #if defined MMU_DEBUG && defined MMU_DEBUG_WRITE
    DEBUG_MMU("Write to %04X <- %x\n", addr, data);
    #endif
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
            result = mmu.boot_rom[addr];
        } else {
            if (emu.rom_info.cartridge_type == ROM_CARTRIDGE_TYPE_ROMONLY) {
                result = emu.rom[addr];
            } else {
                result = mbc_rb(addr);
            }
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
            result = mmu.serial_data;
        } else if (addr == 0xFF02) {
            result = mmu.serial_control.value;
        } else if (addr == 0xFF04) {
            // Timer DIV
            result = timer.div;
        } else if (addr == 0xFF05) {
            // Timer TIMA
            result = timer.tima;
        } else if (addr == 0xFF06) {
            // Timer TMA
            result = timer.tma;
        } else if (addr == 0xFF07) {
            // Timer TAC
            result = timer.tac;
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
        DEBUG_MMU("Illegal read operation (%x)\n", addr);
        #endif
        result = 0;
    }

    #if defined MMU_DEBUG && defined MMU_DEBUG_READ
    DEBUG_MMU("Read from %04X -> %x\n", addr, result);
    #endif

    return result;
}

uint16_t mmu_rw(uint16_t addr)
{
    return mmu_rb(addr) | (mmu_rb(addr + 1) << 8);
}