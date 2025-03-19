
#include "Cpu.h"
#include <iostream>

CPU::CPU(GPU &graphics)
{
    mGraphics = &graphics;
    mPC = 0x200;
}

uint16_t CPU::fetchOpcode(const std::array<uint8_t, MEMORY> &ram)
{
    // Fetch opcode
    uint16_t opcode = (ram.at(mPC) << 8) | (ram.at(mPC + 1)); // Grab 2 bytes and combine them

    // Increment PC
    mPC += 2;

    return opcode;
}

void CPU::decodeOpcode(const uint16_t &opcode)
{
    switch (opcode & 0xF000) // Grab first hex char
    {
    case (0x0000): // Handles all 0x0XXX opcodes
        std::cout << "Received clear screen op\n";

        (*mGraphics).clearScreen();
        break;
    }
}