#include "Chip8.h"

// TODO: Consider moving all constants into their own file

#include <iostream>
#include <memory>

int main()
{
    std::unique_ptr<Chip8> chip8Ptr = std::make_unique<Chip8>();

    std::cout << "Attempting to load file..." << std::endl;

    // Roms
    std::string SpaceInvaders = "../roms/Space Invaders [David Winter].ch8";
    std::string Breakout = "../roms/breakout.rom";
    std::string Pong1 = "../roms/Pong (1 player).ch8";
    std::string Pong2 = "../roms/pong2.rom";
    std::string IBM = "../roms/IBM Logo.ch8";
    std::string Opcode = "../roms/3-corax+.ch8";
    std::string OpcodeAlt = "../roms/3-corax+_alt.ch8";
    std::string Flags = "../roms/4-flags.ch8";
    std::string Quirks = "../roms/5-quirks.ch8";
    std::string Input = "../roms/6-keypad.ch8";

    // std::shared_ptr<Rom> rom = std::make_shared<Rom>(Breakout);

    Rom rom(Breakout);
    chip8Ptr->loadRom(rom);

    chip8Ptr->runEngine();

    return 0;
}
