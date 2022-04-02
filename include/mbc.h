#ifndef _mbc_h
#define _mbc_h

typedef struct mbc_t {
    uint8_t type;

    uint8_t rom_banks;
    uint8_t ram_banks;

    uint8_t rom_bank_number;
    uint8_t ram_bank_number;

    uint8_t* rom;
    uint8_t* ram;
} mbc_t;

extern mbc_t mbc;

uint8_t mbc_rb(uint16_t addr);
void mbc_wb(uint16_t addr, uint8_t data);

#endif