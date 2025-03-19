
#include "CPU.h"
#include <iostream>

CPU::CPU(GPU &graphics)
{
    mGraphics = &graphics;
    mPC = 0x200;
    mStackptr = 0;
}

uint16_t CPU::fetchOpcode(const std::array<uint8_t, MEMORY> &memory)
{
    // Fetch opcode
    uint16_t opcode = (memory.at(mPC) << 8) | (memory.at(mPC + 1)); // Grab 2 bytes and combine them

    // Increment PC
    mPC += 2;

    return opcode;
}

void CPU::decodeOpcode(const uint16_t &opcode)
{
    nibbles.first = opcode & 0xF000;
    nibbles.sec = opcode & 0x0F00;
    nibbles.third = opcode & 0x00F0;
    nibbles.fourth = opcode & 0x000F;

    // printf("%0X %0X %0X %0X\n", nibbles.first, nibbles.sec, nibbles.third, nibbles.fourth);
}

void CPU::executeOpcode()
{
    switch (nibbles.first) // Grab first hex char
    {

    case (0x0000): // Handles all 0x0XXX opcodes
    {

        uint8_t thirdFourth = nibbles.third | nibbles.fourth;

        if (thirdFourth == 0xE0) // Clear screen op (0x00E0)
        {
            std::cout << "Received clear screen op\n";

            (*mGraphics).clearScreen();
        }

        else if (thirdFourth == 0xEE) // Return from subroutine
        {
            std::cout << "Received return call\n";

            mStackptr--;
            mPC = mStack.at(mStackptr);
        }

        break;
    }

    case (0x1000): // Jump instruction; PC jumps to NNN, derived from 0x1NNN
    {
        mPC = nibbles.sec | nibbles.third | nibbles.fourth;

        // printf("%0X\n", mPC);
        break;
    }

    case (0x2000): // Store current PC on stack, then jump to subroutine 0x2NNN
    {
        mStack.at(mStackptr) = mPC;
        mStackptr++; // Should never be greater than 16!
        if (mStackptr > 16)
        {
            std::cout << "mStackptr somehow greater than 16!" << std::endl;
        }

        mPC = nibbles.sec | nibbles.third | nibbles.fourth;
        // printf("%0X\n", mPC);
        // std::cout << std::hex << mStack.at(mStackptr - 1);

        break;
    }
    }
}