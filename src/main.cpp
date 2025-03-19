#include "CHIP8.h"

#include <iostream>

int main()
{
    CHIP8 chip8;

    std::cout << "Attempting to load file..." << std::endl;
    std::string path = "../roms/Pong (1 player).ch8";
    if (chip8.loadRom(path))
    {
        std::cout << "Successfully opened file!" << std::endl;
    }

    // Check memory
    chip8.printMemory();

    return 0;
}