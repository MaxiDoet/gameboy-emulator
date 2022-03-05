#include <iostream>
#include "emulator.h"

int main()
{
    Emulator emu;

    emu.load("rom.gb");
    emu.run();

    return 0;
}