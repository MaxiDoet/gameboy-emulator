#include "emulator.h"

emulator_t emu;

const char* cartridge_type_names[0xFF] = {
    [0x00] = "ROMONLY",
    [0x01] = "MBC1",
    [0x02] = "MBC1+RAM",
    [0x03] = "MBC1+RAM+BATTERY",
    [0x05] = "MBC2",
    [0x06] = "MBC2+BATTERY",
    [0x13] = "MBC3+RAM+BATTERY"
};

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

    memcpy(mmu.rom, emu.rom, (size > 0x8000) ? 0x8000 : size);

    printf("[emulator] Loaded %s (%ld bytes)\n", path, size);

    //debug_mem_dump(0x0000, 0x8000);

    // Title
    memcpy(emu.rom_info.title, &emu.rom[ROM_TITLE_OFFSET], 16);

    // Cartridge type
    emu.rom_info.cartridge_type = emu.rom[ROM_CARTRIDGE_TYPE_OFFSET];
    strcpy(emu.rom_info.cartridge_type_name, cartridge_type_names[emu.rom_info.cartridge_type]);
    mbc.type = emu.rom_info.cartridge_type;

    // ROM banks
    emu.rom_info.rom_banks = 2 << emu.rom[ROM_ROM_SIZE_OFFSET];
    mbc.rom_banks = emu.rom_info.rom_banks;
    mbc.rom = (uint8_t *) malloc(0x4000 * mbc.rom_banks);
    memcpy(mbc.rom, emu.rom, size);

    // RAM banks
    switch(emu.rom[ROM_RAM_SIZE_OFFSET]) {
        // No RAM
        case 0x00:
            emu.rom_info.ram_banks = 0;
            break;

        // 1 bank
        case 0x02:
            emu.rom_info.ram_banks = 1;
            break;

        // 4 banks
        case 0x03:
            emu.rom_info.ram_banks = 4;
            break;

        // 16 banks
        case 0x04:
            emu.rom_info.ram_banks = 16;
            break;

        // 8 banks
        case 0x05:
            emu.rom_info.ram_banks = 8;
            break;
    }

    mbc.ram_banks = emu.rom_info.ram_banks;
    mbc.ram = (uint8_t *) malloc(0x2000 * mbc.ram_banks);

    return 0;
}

void emulator_print_rom_info()
{
    printf("[emulator] ROM info:\n");

    printf("  - Title: %s\n", emu.rom_info.title);
    printf("  - Cartridge type: %s (%02X)\n", emu.rom_info.cartridge_type_name, emu.rom_info.cartridge_type);
    printf("  - ROM banks: %d\n", emu.rom_info.rom_banks);
    printf("  - RAM banks: %d\n", emu.rom_info.ram_banks);
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