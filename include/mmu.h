#ifndef _mmu_h
#define _mmu_h

#include "emulator.h"

#define MMU_DEBUG
//#define MMU_DEBUG_ROM
//#define MMU_DEBUG_IO

void mmu_init();
void mmu_load(uint8_t *data, uint16_t size);
void mmu_wb(uint16_t addr, uint8_t data);
void mmu_ww(uint16_t addr, uint16_t data);
uint8_t mmu_rb(uint16_t addr);
uint16_t mmu_rw(uint16_t addr);

typedef union {
    struct {
        uint8_t shift_clock : 1;
        uint8_t clock_speed : 1;
        uint8_t reserved    : 5;
        uint8_t start_flag  : 1;
    } fields;

    uint8_t value;  
} serial_control_t;

typedef struct mmu_t {
    uint8_t boot_rom[0x0100];
    uint8_t rom[0x8000];
    uint8_t vram[0x2000];
    uint8_t sram[0x2000];
    uint8_t wram[0x2000];
    uint8_t oam[0x0100];
    uint8_t hram[0x007F];

    serial_control_t serial_control;
    bool boot_rom_mapped;
} mmu_t;

extern mmu_t mmu;

#endif