#include "emulator.h"

#include <SDL2/SDL.h>

const char* cartridge_type_names[0xFF] = {
    [0x00] = "ROMONLY",
    [0x01] = "MBC1",
    [0x02] = "MBC1+RAM",
    [0x03] = "MBC1+RAM+BATTERY",
    [0x05] = "MBC2",
    [0x06] = "MBC2+BATTERY",
    [0x13] = "MBC3+RAM+BATTERY"
};

const uint32_t default_palette[4] = {
    0xFFFFFFFF, // White
    0xFF969696, // Light Grey
    0xFF3B3B3B, // Dark Grey
    0xFF000000 // Black
};

/* Original Gameboy palette

uint8_t default_palette[4] = {
    0xFF9BBC0F,
    0xFF8BAC0F,
    0xFF306230,
    0xFF0F380F
};

*/

void load_rom(const char *path)
{
    FILE *rom_fp = fopen(path, "rb");

    fseek(rom_fp, 0, SEEK_END);
    long size = ftell(rom_fp);
    fseek(rom_fp, 0, SEEK_SET);

    emulator.rom = (uint8_t *) malloc(size);
    fread(emulator.rom, size, 1, rom_fp);
    fclose(rom_fp);

    memcpy(mmu.rom, emulator.rom, (size > 0x8000) ? 0x8000 : size);

    printf("[emulator] Loaded %s (%ld bytes)\n", path, size);

    // Title
    memcpy(emulator.rom_info.title, &emulator.rom[ROM_TITLE_OFFSET], 16);

    // Cartridge type
    emulator.rom_info.cartridge_type = emulator.rom[ROM_CARTRIDGE_TYPE_OFFSET];
    strcpy(emulator.rom_info.cartridge_type_name, cartridge_type_names[emulator.rom_info.cartridge_type]);
    
    switch(emulator.rom_info.cartridge_type) {
        case ROM_CARTRIDGE_TYPE_ROMONLY:
            mbc.type = MBC_TYPE_NOMBC;
            break;

        case ROM_CARTRIDGE_TYPE_MBC1:
        case ROM_CARTRIDGE_TYPE_MBC1RAM:
        case ROM_CARTRIDGE_TYPE_MBC1RAMBAT:
            mbc.type = MBC_TYPE_MBC1;
            break;

        case ROM_CARTRIDGE_TYPE_MBC2:
        case ROM_CARTRIDGE_TYPE_MBC2BAT:
            mbc.type = MBC_TYPE_MBC2;
            break;

        case ROM_CARTRIDGE_TYPE_MBC3TIMERBAT:
        case ROM_CARTRIDGE_TYPE_MBC3TIMERRAMBAT:
        case ROM_CARTRIDGE_TYPE_MBC3:
        case ROM_CARTRIDGE_TYPE_MBC3RAM:
        case ROM_CARTRIDGE_TYPE_MBC3RAMBAT:
            mbc.type = MBC_TYPE_MBC3;
            break;

        default:
            mbc.type = MBC_TYPE_NOMBC;
            break;
    }

    // ROM banks
    emulator.rom_info.rom_banks = 2 << emulator.rom[ROM_ROM_SIZE_OFFSET];
    mbc.rom_banks = emulator.rom_info.rom_banks;
    mbc.rom = (uint8_t *) malloc(0x4000 * mbc.rom_banks);
    memcpy(mbc.rom, emulator.rom, size);

    // RAM banks
    switch(emulator.rom[ROM_RAM_SIZE_OFFSET]) {
        // No RAM
        case 0x00:
            emulator.rom_info.ram_banks = 0;
            break;

        // 1 bank
        case 0x02:
            emulator.rom_info.ram_banks = 1;
            break;

        // 4 banks
        case 0x03:
            emulator.rom_info.ram_banks = 4;
            break;

        // 16 banks
        case 0x04:
            emulator.rom_info.ram_banks = 16;
            break;

        // 8 banks
        case 0x05:
            emulator.rom_info.ram_banks = 8;
            break;
    }

    mbc.ram_banks = emulator.rom_info.ram_banks;
    mbc.ram = (uint8_t *) malloc(0x2000 * mbc.ram_banks);

    mbc_init();
}

void render()
{
    /*
    for (int window_y=0; window_y < LCD_HEIGHT * LCD_SCALE; window_y++) {
        for (int window_x=0; window_x < LCD_WIDTH * LCD_SCALE; window_x++) {
            int screen_x = window_x / LCD_SCALE;
            int screen_y = window_y / LCD_SCALE;

            SDL_SetRenderDrawColor(emulator.renderer, 
                lcd.framebuffer[screen_y * LCD_WIDTH + screen_x][0],
                lcd.framebuffer[screen_y * LCD_WIDTH + screen_x][1],
                lcd.framebuffer[screen_y * LCD_WIDTH + screen_x][2],
                0xFF
            );

            SDL_RenderDrawPoint(emulator.renderer, window_x, window_y);
        }
    }
    */

    handle_events();

    void* pixels_ptr;
    int pitch;
    SDL_LockTexture(emulator.texture, NULL, &pixels_ptr, &pitch);

    uint32_t *pixels = (uint32_t *) pixels_ptr;

    for (int y=0; y < LCD_HEIGHT; y++) {
        for (int x=0; x < LCD_WIDTH; x++) {
            pixels[y * LCD_WIDTH + x] = default_palette[lcd.color_buffer[y * LCD_WIDTH + x]];
        }
    }

    SDL_UnlockTexture(emulator.texture);

    SDL_RenderCopy(emulator.renderer, emulator.texture, NULL, NULL);
    SDL_RenderPresent(emulator.renderer);
}

void handle_events()
{
    // Handle SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                emulator.running = false;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                input_handle(&event.key);
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[])
{    
    // Init SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
        printf("Unable to init SDL!\n");
        exit(-1);
    }

    emulator.window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LCD_WIDTH * LCD_SCALE, LCD_HEIGHT * LCD_SCALE, SDL_WINDOW_OPENGL);

    if (!emulator.window) {
        printf("Unable to create window!\n");
        exit(-1);
    }

    emulator.renderer = SDL_CreateRenderer(emulator.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!emulator.renderer) {
        printf("Unable to create renderer!\n");
        exit(-1);
    }

    emulator.texture = SDL_CreateTexture(emulator.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, LCD_WIDTH, LCD_HEIGHT);

    if (!emulator.texture) {
        printf("Unable to create texture!\n");
        exit(-1);
    }

    /*
    // Init audio
    SDL_AudioSpec audio_spec;
    SDL_zero(audio_spec);

    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_F32SYS;
    audio_spec.channels = 2;
    audio_spec.samples = 100;

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

    uint8_t *buf = (uint8_t *) malloc(100);
    memset(buf, 0xFF, 100);

    SDL_QueueAudio(deviceId, buf, 100);
    SDL_PauseAudioDevice(deviceId, 0);
    SDL_Delay(3000);
    */

    // Init emulator
    cpu_init();
    mmu_init();
    lcd_init();
    input_init();
    sound_init();

    if (argc != 1) {
        load_rom(argv[1]);
    }

    emulator.running = true;
    uint32_t last_cycles = 0;
    while (emulator.running) {
        uint64_t start = SDL_GetPerformanceCounter();
        
		//SDL_RenderClear(emulator.renderer);

        if (cpu.stopped) {
            handle_events();
        }

        cpu_step();
        timer_tick(cpu.cycles - last_cycles);
        cpu_serve_interrupts();
        lcd_step(cpu.cycles - last_cycles);
        sound_tick(cpu.cycles - last_cycles);
        last_cycles = cpu.cycles;

        uint64_t end = SDL_GetPerformanceCounter();
    }

    SDL_Quit();
}