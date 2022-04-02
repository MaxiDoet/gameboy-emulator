#include "emulator.h"

#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{    
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Unable to init SDL!\n");
        exit(-1);
    }

    emulator_init();

    if (argc != 1) {
        emulator_load(argv[1]);
    }

    emulator_print_rom_info();

    bool running = true;
    SDL_Event event;
    uint32_t last_cycles = 0;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    input_handle(&event.key);
                    break;
            }
        }

        cpu_step();
        lcd_step(cpu.cycles - last_cycles);
        timer_tick(cpu.cycles - last_cycles);

        last_cycles = cpu.cycles;
    }

    SDL_Quit();
    return 0;
}