#include "emulator.h"

mbc_t mbc;

/* Todo: Define own MBC types */

uint8_t mbc_rb(uint16_t addr)
{
    uint8_t result = 0;

    if (addr <= 0x3FFF) {
        // Fixed rom bank except for MBC1

        if (mbc.type == ROM_CARTRIDGE_TYPE_MBC1) {
            // Switchable bank
        } else {
            result = mbc.rom[addr];
        }
    } else if (addr >= 4000 && addr <= 0x7FFF) {
        // Switchable rom bank
        result = mbc.rom[0x4000 * mbc.rom_bank_number];
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        // Switchable ram bank
        result = mbc.ram[0x2000 * mbc.ram_bank_number];
    }

    return result;
}

void mbc_wb(uint16_t addr, uint8_t data)
{
    // ROM bank number
    if (addr >= 0x2000 && addr <= 0x3FFF) {
        printf("bank: %d\n", data);
        if (data <= mbc.rom_banks) {
            if (data == 0x00) {
                mbc.rom_bank_number = 0x01;
            } else {
                mbc.rom_bank_number = data;
            }
        }
    }
}