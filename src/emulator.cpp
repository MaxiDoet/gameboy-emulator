#include "emulator.h"
#include <iostream>
#include <fstream>

Emulator::Emulator()
{
    this->cpu = Cpu();
    this->mmu = MMU();
}

void Emulator::run()
{

}

int Emulator::load(const char *path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        return -1;
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);

    uint8_t *buf = (uint8_t *) malloc(size);
    this->mmu.load(buf, size);

    std::cout << "[emulator] Loaded rom " << path << " (" << size << " bytes)" << std::endl;

    file.close();

    return 0;
}