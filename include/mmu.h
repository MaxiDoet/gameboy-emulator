#ifndef _mmu_h
#define _mmu_h

#include <cstdint>
#include <string.h>

class MMU {
public:
    MMU();

    void load(uint8_t *data, uint16_t size);

    uint8_t rom[0x7FFF];
    uint8_t vram[0x1FFF];
    uint8_t wram[0x0FFF];
    uint8_t oam[0x009F];
    uint8_t hram[0x007E];    
};

#endif