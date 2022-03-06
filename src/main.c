#include "emulator.h"

int main()
{
    emulator_load("rom.gb");
    emulator_run();

    return 0;
}