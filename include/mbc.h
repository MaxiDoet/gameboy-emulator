#ifndef _mbc_h
#define _mbc_h

#define MBC_TYPE_NOMBC  0
#define MBC_TYPE_MBC1   1
#define MBC_TYPE_MBC2   2
#define MBC_TYPE_MBC3   3
#define MBC_TYPE_MBC5   4
#define MBC_TYPE_MBC6   5
#define MBC_TYPE_MBC7   6

typedef struct mbc_t {
    uint8_t type;

    uint8_t rom_banks;
    uint8_t ram_banks;

    uint8_t rom_bank_number;
    uint8_t ram_bank_number;

    uint8_t* rom;
    uint8_t* ram;

    // MBC1
    int banking_mode;
    
    bool ram_enabled;
    bool rtc_enabled;
} mbc_t;

extern mbc_t mbc;

uint8_t mbc_rb(uint16_t addr);
void mbc_wb(uint16_t addr, uint8_t data);

#endif