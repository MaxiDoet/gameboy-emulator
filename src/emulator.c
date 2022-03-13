#include "emulator.h"

emu_t emu;

void emulator_init()
{
    cpu_init();
    lcd_init();

    // Init SDL
    emu.window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, 0);

    if (!emu.window) {
        printf("Unable to create window!\n");
        exit(-1);
    }

    emu.renderer = SDL_CreateRenderer(emu.window, -1, SDL_RENDERER_ACCELERATED);
}

int emulator_load(const char *path)
{
    FILE *rom = fopen(path, "rb");

    fseek(rom, 0, SEEK_END);
    long size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    emu.rom = (uint8_t *) malloc(size);
    fread(emu.rom, size, 1, rom);
    memcpy(mmu.rom, emu.rom, 0x7FFF);

    printf("[emulator] Loaded %s (%ld bytes)\n", path, size);

    return 0;
}

void emulator_rom_info()
{
    char title[16];

    printf("Rom:\n");

    memcpy(title, &mmu.rom[ROM_TITLE_START], 16);
    printf("  - Title: %s\n", title);

    printf("  - Licensee code: %c%c\n", (char) mmu.rom[ROM_PUBLISHER_CODE_START], (char) mmu.rom[ROM_PUBLISHER_CODE_START + 1]);
}