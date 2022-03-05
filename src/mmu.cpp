#include "mmu.h"

MMU::MMU()
{
    
}

void MMU::load(uint8_t *data, uint16_t size)
{
    memcpy(rom, data, size);
}

