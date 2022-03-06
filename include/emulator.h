#ifndef _emulator_h
#define _emulator_h

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "mmu.h"

void emulator_init();
int emulator_load(const char *path);
void emulator_run();

#endif