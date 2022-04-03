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

    emulator_run();

    SDL_Quit();
    return 0;
}