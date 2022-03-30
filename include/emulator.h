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
#include "debug.h"
#include "boot.h"

typedef struct emu_t {
    uint8_t *rom;
    SDL_Window *window;
    SDL_Renderer *renderer;
} emu_t;

void emulator_init();
int emulator_load(const char *path);
void emulator_rom_info();
void emulator_render();

extern emu_t emu;

#endif