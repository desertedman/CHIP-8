#include "Chip8.h"

#include <iostream>

int main()
{
    Chip8 chip8;

    std::cout << "Attempting to load file..." << std::endl;
    std::string path = "../roms/Pong (1 player).ch8";
    if (chip8.loadRom(path))
    {
        std::cout << "Successfully opened file!" << std::endl;
    }

    else
    {
        std::cout << "Failed to open file! Closing." << std::endl;
        return -1;
    }

    // chip8.drawScreen(); // Screen starts immediately filled
    // std::cout << "\n";
    // chip8.cycleCPU(); // Clear screen
    // chip8.drawScreen();

    chip8.cycleCPU();

    return 0;
}