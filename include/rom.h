#ifndef _rom_h
#define _rom_h

#define ROM_TITLE_OFFSET                0x134
#define ROM_CARTRIDGE_TYPE_OFFSET       0x147
#define ROM_ROM_SIZE_OFFSET             0x148
#define ROM_RAM_SIZE_OFFSET             0x149

#define ROM_CARTRIDGE_TYPE_ROMONLY          0x00
#define ROM_CARTRIDGE_TYPE_MBC1             0x01
#define ROM_CARTRIDGE_TYPE_MBC1RAM          0x02
#define ROM_CARTRIDGE_TYPE_MBC1RAMBAT       0x03
#define ROM_CARTRIDGE_TYPE_MBC2             0x05
#define ROM_CARTRIDGE_TYPE_MBC2BAT          0x06
#define ROM_CARTRIDGE_TYPE_MBC3TIMERBAT     0x0F
#define ROM_CARTRIDGE_TYPE_MBC3TIMERRAMBAT  0x10
#define ROM_CARTRIDGE_TYPE_MBC3             0x11
#define ROM_CARTRIDGE_TYPE_MBC3RAM          0x12
#define ROM_CARTRIDGE_TYPE_MBC3RAMBAT       0x13

typedef struct rom_info_t {
    char title[16];
    uint8_t cartridge_type;
    char cartridge_type_name[32];
    uint8_t rom_banks;
    uint8_t ram_banks;
    
    bool cgb;
    bool sgb;
} rom_info_t;

#endif