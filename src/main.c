#include "emulator.h"

#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Unable to init SDL!\n");
        exit(-1);
    }

    emulator_init();
    emulator_load(argv[1]);
    emulator_rom_info();

    bool running = true;
    SDL_Event event;
    uint32_t last_cycles = 0;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        cpu_step();
        lcd_step(cpu.cycles - last_cycles);

        last_cycles = cpu.cycles;
    }

    SDL_Quit();
    return 0;
}