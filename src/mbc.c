#include "emulator.h"

#ifdef MBC_DEBUG
#define DEBUG_MBC(...) printf("[mbc] "); printf(__VA_ARGS__)
#endif

mbc_t mbc;

void mbc_init()
{
    if (mbc.type == MBC_TYPE_MBC1) {
        mbc.rom_bank = 0x01;
    }
}

uint8_t mbc_rb(uint16_t addr)
{
    uint8_t result = 0;

    if (mbc.type == MBC_TYPE_MBC1) {
        if (addr <= 0x3FFF) {
            if (mbc.banking_mode == 0) {
                result = mbc.rom[addr];
            } else if (mbc.banking_mode == 1) {
                if (mbc.rom_bank == 0x20 || mbc.rom_bank == 0x40 || mbc.rom_bank == 0x60) {
                    result = mbc.rom[0x4000 * mbc.rom_bank + addr];
                }
            }
        } else if (addr >= 0x4000 && addr <= 0x7FFF) {
            uint8_t rom_bank = (mbc.ram_bank << 5) + mbc.rom_bank;

            if (!mbc.rom_bank) {
                rom_bank++;
            }

            // Switchable rom bank
            result = mbc.rom[0x4000 * rom_bank + (addr - 0x4000)];
        } else if (addr >= 0xA000 && addr <= 0xBFFF) {
            if (mbc.ram_enabled) {
                result = mbc.ram[0x2000 * (mbc.ram_bank & 3) + (addr - 0x4000)];
            }
        }
    }

    return result;
}

void mbc_wb(uint16_t addr, uint8_t data)
{
    if (mbc.type == MBC_TYPE_MBC1) {
        if (addr <= 0x1FFF) {
            mbc.ram_enabled = (data & 0xF) == 0xA; 

            DEBUG_MBC("MBC1: %s RAM\n", mbc.ram_enabled ? "Enabled" : "Disabled");
        } else if (addr >= 0x2000 && addr <= 0x3FFF) {
            if (data == 0x00) {
                mbc.rom_bank = 1;
            } else {
                mbc.rom_bank = (data & 0x1F);
            }

            DEBUG_MBC("MBC1: Selected rom bank: %d\n", mbc.rom_bank);
        } else if (addr >= 0x4000 && addr <= 0x5FFF) {
            mbc.ram_bank = data;

            DEBUG_MBC("MBC1: Selected ram bank: %d\n", mbc.rom_bank & 3);
        } else if (addr >= 0x6000 && addr <= 0x7FFF) {
            mbc.banking_mode = data & 0x01;

            DEBUG_MBC("MBC1: Set banking mode: %d\n", mbc.banking_mode);
        } else if (addr >= 0xA000 && addr <= 0xBFFF) {
            if (mbc.ram_enabled) {
                mbc.ram[0x2000 * (mbc.ram_bank & 3) + (addr - 0x4000)] = data;
            }
        }
    }
}
