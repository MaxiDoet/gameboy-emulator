#ifndef _mmu_h
#define _mmu_h

#include "emulator.h"

void mmu_load(uint8_t *data, uint16_t size);
void mmu_wb(uint16_t addr, uint8_t data);
void mmu_ww(uint16_t addr, uint16_t data);
uint8_t mmu_rb(uint16_t addr);
uint16_t mmu_rw(uint16_t addr);

typedef struct mmu_t {
    uint8_t rom[0x7FFF];
    uint8_t vram[0x1FFF];
    uint8_t wram[0x0FFF];
    uint8_t oam[0x009F];
    uint8_t hram[0x007E];
} mmu_t;

extern mmu_t mmu;

#endif