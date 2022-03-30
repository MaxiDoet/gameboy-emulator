#include "emulator.h"

emu_t emu;

void emulator_init()
{
    cpu_init();
    mmu_init();
    lcd_init();
    input_init();

    // Init SDL
    emu.window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LCD_WIDTH * LCD_SCALE, LCD_HEIGHT * LCD_SCALE, 0);

    if (!emu.window) {
        printf("Unable to create window!\n");
        exit(-1);
    }

    emu.renderer = SDL_CreateRenderer(emu.window, -1, SDL_RENDERER_ACCELERATED);
}

int emulator_load(const char *path)
{
    FILE *rom_fp = fopen(path, "rb");

    fseek(rom_fp, 0, SEEK_END);
    long size = ftell(rom_fp);
    fseek(rom_fp, 0, SEEK_SET);

    emu.rom = (uint8_t *) malloc(size);
    fread(emu.rom, size, 1, rom_fp);
    memcpy(mmu.rom, emu.rom, size);

    printf("[emulator] Loaded %s (%ld bytes)\n", path, size);

    return 0;
}

void emulator_rom_info()
{
    char title[16];

    printf("[emulator] ROM info:\n");

    memcpy(title, &mmu.rom[ROM_TITLE_START], 16);
    printf("  - Title: %s\n", title);
    printf("  - Cartridge Type: %x\n", mmu.rom[ROM_CARTRIDGE_TYPE]); 
    printf("  - Licensee code: %c%c\n", (char) mmu.rom[ROM_PUBLISHER_CODE_START], (char) mmu.rom[ROM_PUBLISHER_CODE_START + 1]);
}

void emulator_render()
{
    
    for (int window_y=0; window_y < LCD_HEIGHT * LCD_SCALE; window_y++) {
        for (int window_x=0; window_x < LCD_WIDTH * LCD_SCALE; window_x++) {
            int screen_x = window_x / LCD_SCALE;
            int screen_y = window_y / LCD_SCALE;

            SDL_SetRenderDrawColor(emu.renderer, 
                lcd.pixels[screen_y * LCD_WIDTH + screen_x][0],
                lcd.pixels[screen_y * LCD_WIDTH + screen_x][1],
                lcd.pixels[screen_y * LCD_WIDTH + screen_x][2],
                0xFF
            );

            SDL_RenderDrawPoint(emu.renderer, window_x, window_y);
        }
    }

    /*
    for (int y=0; y < 144; y++) {
        for (int x=0; x < 160; x++) {
            SDL_SetRenderDrawColor(emu.renderer, 
                lcd.pixels[y * LCD_WIDTH + x][0],
                lcd.pixels[y * LCD_WIDTH + x][1],
                lcd.pixels[y * LCD_WIDTH + x][2],
                0xFF
            );
            
            SDL_RenderDrawPoint(emu.renderer, x, y);
        }
    }
    */

    SDL_RenderPresent(emu.renderer);
}