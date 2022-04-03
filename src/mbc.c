#include "emulator.h"

mbc_t mbc;

/* Todo: Define own MBC types */

uint8_t mbc_ram_rb(uint16_t addr)
{
    uint8_t result;

    if (mbc.type == MBC_TYPE_MBC1 || mbc.type == MBC_TYPE_MBC3) {
        result = mbc.ram[0x2000 * mbc.rom_bank_number + addr - 0xA000];
    }

    return result;
}

void mbc_ram_wb(uint16_t addr, uint8_t data)
{
    if (mbc.type == MBC_TYPE_MBC1 || mbc.type == MBC_TYPE_MBC3) {
        mbc.ram[0x2000 * mbc.rom_bank_number + addr - 0xA000] = data;
    }
}

uint8_t mbc_rb(uint16_t addr)
{
    uint8_t result = 0;

    if (addr <= 0x3FFF) {
        // Fixed rom bank except for MBC1

        if (mbc.type == MBC_TYPE_MBC1) {
            // Switchable bank
        } else {
            result = mbc.rom[addr];
        }
    } else if (addr >= 4000 && addr <= 0x7FFF) {
        // Switchable rom bank
        result = mbc.rom[0x4000 * mbc.rom_bank_number];
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        // Switchable ram bank
        result = mbc_ram_rb(addr);
    }

    return result;
}

void mbc_wb(uint16_t addr, uint8_t data)
{
    if (addr <= 0x1FFF) {
        // RAM and Timer enable
        

    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        if (data <= mbc.rom_banks) {
            if (data == 0x00) {
                mbc.rom_bank_number = 0x01;
            } else {
                mbc.rom_bank_number = data;
            }
        }
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        mbc_ram_wb(addr, data);
    }
}
