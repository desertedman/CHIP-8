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

    chip8.mGraphics.fillScreen();
    chip8.mGraphics.printScreen();

    std::cout << "\n";
    chip8.cycle();
    chip8.mGraphics.printScreen();

    return 0;
}