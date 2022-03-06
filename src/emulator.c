#include "emulator.h"

void emulator_run()
{
    bool running = true;

    while (running) {
        cpu_step();
    }
}

int emulator_load(const char *path)
{
    FILE *rom = fopen(path, "rb");

    fseek(rom, 0, SEEK_END);
    long size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    fread(mmu.rom, size, 1, rom);

    printf("[emulator] Loaded %s (%ld bytes)\n", path, size);

    return 0;
}