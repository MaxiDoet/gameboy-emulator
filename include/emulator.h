#ifndef _emulator_h
#define _emulator_h

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "cpu.h"
#include "mmu.h"
#include "rom.h"
#include "lcd.h"
#include "input.h"
#include "timer.h"
#include "sound.h"
#include "mbc.h"
#include "debug.h"
#include "boot.h"

typedef struct emulator_t {
    uint8_t *rom;
    rom_info_t rom_info;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    bool running;
} emulator_t;

#define CYCLES_PER_SECOND 4194304
#define CYCLES_PER_FRAME 69905

void handle_events();
void render();

extern emulator_t emulator;

#endif