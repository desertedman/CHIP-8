#include "Chip8.h"
#include "Display.h"

// TODO: Consider moving all constants into their own file

#include <SDL2/SDL.h>
#include <iostream>

int main()
{
    Chip8 chip8;
    if (!chip8.initialize())
    {
        std::cout << "Failed to initialize, exiting...\n";
        return -1;
    }

    std::cout << "Attempting to load file..." << std::endl;
    std::string Pong1 = "../roms/Pong (1 player).ch8";
    std::string Pong2 = "../roms/pong2.rom";
    std::string IBM = "../roms/IBM Logo.ch8";
    std::string Test = "../roms/test_opcode.ch8";
    if (chip8.loadRom(Pong1))
    {
        std::cout << "Successfully opened file!" << std::endl;
    }

    else
    {
        std::cout << "Failed to open file! Closing." << std::endl;
        return -1;
    }

    // chip8.printMemory();
    // chip8.testEngine();
    chip8.runEngine();

    return 0;
}