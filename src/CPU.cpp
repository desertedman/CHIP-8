
#include "CPU.h"
#include <iostream>

// CPU::CPU(std::array<uint8_t, MEMORY> &memory)
// {
//     // I = &memory.at(0);
// }

void CPU::initialize()
{
    mPC = 0x200;
    mStackptr = 0;

    mStack.fill(false); // Clear stack; probably unnecessary

    // Reset nibbles; unnecessary?
    nibbles.first = 0;
    nibbles.sec = 0;
    nibbles.third = 0;
    nibbles.fourth = 0;

    // Set memory pointer
    I = 0;
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

void CPU::executeOpcode(GPU &gpu, std::array<uint8_t, MEMORY> &memory)
{
    switch (nibbles.first) // Grab first hex char
    {

    case (0x0000): // Handles all 0x0XXX opcodes
    {

        uint8_t thirdFourth = nibbles.third | nibbles.fourth;

        if (thirdFourth == 0xE0) // Clear screen op (0x00E0)
        {
            std::cout << "Received clear screen op\n";

            gpu.clearScreen();
        }

        else if (thirdFourth == 0xEE) // Return from subroutine
        {
            std::cout << "Received return call\n";

            // Pop stack and jump to address
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

    case (0x3000): // 0x3XNN; if VX == NN, skip one instruction
    {
        if (V[nibbles.sec >> 8] == (nibbles.third | nibbles.fourth))
        {
            mPC += 2;
        }

        break;
    }

    case (0x4000): // 0x3XNN; if VX != NN, skip one instruction
    {
        if (V[nibbles.sec >> 8] != (nibbles.third | nibbles.fourth))
        {
            mPC += 2;
        }

        break;
    }

    case (0x5000): // 0x5XY0; if VX == VY, skip
    {
        if (V[nibbles.sec >> 8] == V[nibbles.third] >> 4)
        {
            mPC += 2;
        }

        break;
    }

    case (0x6000): // 0x6XNN; set VX = NN
    {
        V[nibbles.sec >> 8] = (nibbles.third | nibbles.fourth);

        break;
    }

    case (0x7000): // 0x7XNN; Add NN to VX. Does not set carry flag
    {
        V[nibbles.sec >> 8] += (nibbles.third | nibbles.fourth);

        break;
    }

    case (0x8000):
    {
        if (nibbles.fourth == 0x04) // 0x8XY4; Adds value of VY to VX
        {
            // If VX + VY > 0xFF (max value for unsigned 8bit), then set carry flag
            // Adjust equation to: VX > 0xFF - VY

            if (V[nibbles.third >> 4] > (0xFF - V[nibbles.sec >> 8]))
            {
                V[0xF] = 1; // Set carry flag
            }

            else
            {
                V[0xF] = 0;
            }

            V[nibbles.sec >> 8] += V[nibbles.third >> 4];
            mPC += 2;
        }

        break;
    }

    case (0x9000): // 0x9XY0; if VX != VY, skip
    {
        if (V[nibbles.sec >> 8] != V[nibbles.third] >> 4)
        {
            mPC += 2;
        }

        break;
    }

    case (0xA000): // 0xINNN; Set index register I to 0xNNN
    {
        I = nibbles.sec | nibbles.third | nibbles.fourth;

        break;
    }

    case (0xD000): // 0xDXYN
        // Refer to:
        // https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font
        // https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
        {
            uint8_t x = V[nibbles.sec >> 8] % 64; // X, Y coordinates
            uint8_t y = V[nibbles.third >> 4] % 64;
            uint8_t height = nibbles.fourth; // This is our N pixels

            // Screen X coordinates range from 0 - 63
            // To make coordinates wrap, set X = X % 64. This gives remainder which is our new coordinate

            for (int yLine = 0; yLine < ROWS; yLine++)
            {
                uint8_t sprite = memory.at(I + yLine);

                for (int xLine = 0; xLine < 8; xLine++)
                {
                    if (x > COLUMNS - 1)
                    {
                        break;
                    }

                    uint8_t pixel = sprite & (0x80 >> xLine); // Grab each bit from left to right. Note that 0x80 is 0b1000 0000
                    if (pixel && gpu.getPixel(x, y))          // Compare bit against current screen pixel
                    {
                        pixel = 0;
                        V[0xF] = 1;
                    }

                    else if (pixel && !gpu.getPixel(x, y))
                    {
                        gpu.setPixel(x, y, true);
                    }

                    x++;
                }

                y++;
            }
            
            break;
        }

    case (0xF000):
    {
        // 0xFX33; see https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
        // under "Example 3: Opcode 0xFX33"
        if ((nibbles.third | nibbles.fourth) == 0x33)
        {
            memory.at(I) = V[nibbles.sec >> 8] / 100;
            memory.at(I + 1) = (V[nibbles.sec >> 8] / 10) % 10;
            memory.at(I + 2) = (V[nibbles.sec >> 8] % 100) % 10;
            mPC += 2;
        }

        break;
    }
    }
}