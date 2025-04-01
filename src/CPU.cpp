
#include "CPU.h"
#include <iostream>
#include <random>

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

    drawFlag = false;

    delayTimer = 0;
    soundTimer = 0;
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
            drawFlag = true;

            // std::cout << "Received clear screen op\n";

            gpu.clearScreen();
        }

        else if (thirdFourth == 0xEE) // Return from subroutine
        {
            // std::cout << "Received return call\n";

            // Pop stack and jump to address
            mStackptr--;
            mPC = mStack.at(mStackptr);

            // std::cout << "Current PC after return: " << std::hex << mPC << std::endl;
        }

        break;
    }

    case (0x1000): // Jump instruction; PC jumps to NNN, derived from 0x1NNN
    {
        mPC = nibbles.sec | nibbles.third | nibbles.fourth;

        // std::cout << "Jumping to address: " << std::hex << mPC << std::endl;
        break;
    }

    case (0x2000): // Store current PC on stack, then jump to subroutine 0x2NNN
    {
        // std::cout << "Current PC before jump: " << std::hex << mPC << std::endl;

        // BELOW STATEMENT IS WRONG!! DO NOT LISTEN
        // PC should remain constant, so decrement to counteract the increment in earlier fetch stage
        // mPC -= 2;

        mStack.at(mStackptr) = mPC;
        mStackptr++; // Should never be greater than 16!
        if (mStackptr > 16)
        {
            std::cout << "mStackptr somehow greater than 16!" << std::endl;
        }

        mPC = nibbles.sec | nibbles.third | nibbles.fourth;

        // std::cout << "Current PC after jump: " << std::hex << mPC << std::endl;
        // std::cout << "Address stored on stack: " << std::hex << mStack.at(mStackptr - 1) << std::endl;

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

        // std::cout << "Register V" << std::hex << (nibbles.sec >> 8) << ": " << (nibbles.third | nibbles.fourth) << std::endl;
        break;
    }

    case (0x7000): // 0x7XNN; Add NN to VX. Does not set carry flag
    {
        V[nibbles.sec >> 8] += (nibbles.third | nibbles.fourth);

        // std::cout << "Register V" << std::hex << (nibbles.sec >> 8) << " after: " << static_cast<int>(V[nibbles.sec >> 8]) << std::endl;
        break;
    }

    // Some Opcodes not implemented yet
    case (0x8000):
    {
        switch (nibbles.fourth)
        {
        case (0x00): // 0x8XY0; Set VX = VY
        {
            V[nibbles.sec >> 8] = V[nibbles.third >> 4];
            break;
        }

        case (0x01): // 0x8XY1; VX = VX | VY
        {
            V[nibbles.sec >> 8] |= V[nibbles.third >> 4];
            break;
        }

        case (0x02): // 0x8XY2; VX = VX & VY
        {
            V[nibbles.sec >> 8] &= V[nibbles.third >> 4];
            break;
        }

        case (0x03): // 0x8XY3; VX = VX ^ VY
        {
            V[nibbles.sec >> 8] ^= V[nibbles.third >> 4];
            break;
        }

        case (0x04): // 0x8XY4; VX = VX + VY
        {
            // Perform mathematical operation first, then carry flag
            V[nibbles.sec >> 8] += V[nibbles.third >> 4];

            // If VX + VY > 0xFF (max value for unsigned 8bit), then set carry flag
            // Adjust equation to: VX > 0xFF - VY
            if (V[nibbles.sec >> 8] > (0xFF - V[nibbles.third >> 4]))
            {
                V[0xF] = 1; // Set carry flag
            }

            else
            {
                V[0xF] = 0;
            }

            break;
        }

        case (0x05): // 0x8XY5; VX = VX - VY
        {
            // Perform math operation first, then carry
            V[nibbles.sec >> 8] -= V[nibbles.third >> 4];

            // If VX > VY, VF = 1; if VY < VX, VF = 0
            // Think about it like this: VF is set to 1 prior to operation.
            // If VX > VY, then no need to "borrow" from VF, leaving VF at 1.
            // If VX < VY, then we borrow from VF, setting VF t0 0.
            if (V[nibbles.sec >> 8] > V[nibbles.third >> 4])
            {
                V[0xF] = 1;
            }

            else
            {
                V[0xF] = 0;
            }

            break;
        }

        case (0x06): // 0x8XY6; Bit shift right
        {
            // Modern behavior; VX = VY
            V[nibbles.sec >> 8] = V[nibbles.third >> 4];

            V[0xF] = V[nibbles.sec >> 8] & 0b00000001; // Store last bit into register
            V[nibbles.sec >> 8] >>= 1;                 // Bitshift right in place

            break;
        }

        case (0x07): // 0x8XY7; VX = VY - VX
        {
            // Perform math operation first, then carry
            V[nibbles.sec >> 8] = V[nibbles.third >> 4] - V[nibbles.sec >> 8];

            // If VY > VX, VF = 1; if VY < VX, VF = 0
            // Think about it like this: VF is set to 1 prior to operation.
            // If VY > VX, then no need to "borrow" from VF, leaving VF at 1.
            // If VY < VY, then we borrow from VF, setting VF t0 0.
            if (V[nibbles.third >> 4] > V[nibbles.sec >> 8])
            {
                V[0xF] = 1;
            }

            else
            {
                V[0xF] = 0;
            }

            break;
        }

        case (0x0E): // 0x8XYE; Bit shift left
        {
            // Modern behavior; VX = VY
            V[nibbles.sec >> 8] = V[nibbles.third >> 4];

            V[0xF] = V[nibbles.sec >> 8] >> 7; // Store first bit into register
            V[nibbles.sec >> 8] <<= 1;         // Bitshift left in place

            break;
        }

        default:
            std::cout << "Error! Opcode " << std::hex << (nibbles.first | nibbles.sec | nibbles.third | nibbles.fourth) << " not implemented.\n";
            break;
        }

        break;
    }

    case (0x9000): // 0x9XY0; if VX != VY, skip
    {
        if (V[nibbles.sec >> 8] != V[nibbles.third >> 4])
        {
            mPC += 2;
        }

        break;
    }

    case (0xA000): // 0xINNN; Set index register I to 0xNNN
    {
        I = nibbles.sec | nibbles.third | nibbles.fourth;

        // std::cout << std::hex << I << std::endl;
        break;
    }

    case (0xB000): // 0xBNNN; PC = NNN + V0
    {
        mPC = (nibbles.sec | nibbles.third | nibbles.fourth) + V[0];

        break;
    }

    case (0xC000): // 0xCXNN; set VX to a random number, AND'd with NN
    {
        V[nibbles.sec >> 8] = (rand() % (0xFF + 1)) & (nibbles.third | nibbles.fourth);

        break;
    }

    case (0xD000): // 0xDXYN
        // Refer to:
        // https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font
        // https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
        {
            drawFlag = true;

            uint8_t x = V[nibbles.sec >> 8] % 64; // X, Y coordinates
            uint8_t y = V[nibbles.third >> 4] % 32;
            uint8_t N = nibbles.fourth; // This is our N pixels

            // Screen X coordinates range from 0 - 63
            // To make coordinates wrap, set X = X % 64. This gives remainder which is our new coordinate

            V[0xF] = 0;
            for (int yLine = 0; yLine < N; yLine++)
            {
                if (y + yLine > ROWS - 1)
                {
                    break;
                }

                uint8_t sprite = memory.at(I + yLine); // Grab sprite data

                for (int xLine = 0; xLine < 8; xLine++)
                {
                    if (x + xLine > COLUMNS - 1)
                    {
                        break;
                    }

                    uint8_t pixel = sprite & (0x80 >> xLine); // Grab each pixel bit from left to right. Note that 0x80 is 0b1000 0000
                    if (pixel)                                // Compare bit against current screen pixel
                    {
                        if (gpu.getPixel(x + xLine, y + yLine))
                        {
                            V[0xF] = 1;
                        }

                        gpu.xorPixel(x + xLine, y + yLine, 1);
                    }

                    // DO NOT INCREMENT X!!
                    // x++;
                }

                // DO NOT INCREMENT Y!!
                // y++;
            }

            break;
        }

    case (0xE000): // Input opcodes
    {
        // 0xEX9E; check if key in VX is pressed or not. If it is, then skip an instruction
        if ((nibbles.third | nibbles.fourth) == 0x9E)
        {
            if (mInternalKeys[V[nibbles.sec >> 8]] == true)
            {
                std::cout << "Key detected\n";
                mPC += 2;
            }
        }

        // 0xEXA1; check if key in VX is pressed or not. If not, then skip an instruction
        else if ((nibbles.third | nibbles.fourth) == 0xA1)
        {
            if (mInternalKeys[V[nibbles.sec >> 8]] == false)
            {
                mPC += 2;
            }
        }
    }

    case (0xF000):
    {
        if ((nibbles.third | nibbles.fourth) == 0x07) // 0xFX07; VX = delayTimer
        {
            V[nibbles.sec >> 8] = delayTimer;
        }

        // 0xFX0A; Wait on any key input. Loop until we receive an input
        else if ((nibbles.third | nibbles.fourth) == 0x0A)
        {
            // std::cout << "Waiting on input; 0xFX0A...\n";
            bool anyKeyPressed = false;

            for (int i = 0; i < NUM_KEYS; i++)
            {
                if (mInternalKeys[i] == true)
                {
                    anyKeyPressed = true;
                    std::cout << "Key " << i << " is pressed!";

                    // Send hexadecimal value of char to VX
                    V[nibbles.sec >> 8] = i;
                }
            }

            if (!anyKeyPressed)
            {
                mPC -= 2; // If no input is received, loop this instruction
            }
        }

        else if ((nibbles.third | nibbles.fourth) == 0x15) // 0xFX15; delayTimer = VX
        {
            delayTimer = V[nibbles.sec >> 8];
        }

        else if ((nibbles.third | nibbles.fourth) == 0x18) // 0xFX18; soundTimer = VX
        {
            soundTimer = V[nibbles.sec >> 8];
        }

        else if ((nibbles.third | nibbles.fourth) == 0x1E) // 0xFX1E; I += VX
        {
            I += V[nibbles.sec >> 8];

            // If VX + I > 1000, then set carry flag
            // Adjust equation to: VX > 1000 - I
            // if (V[nibbles.sec >> 8] > 1000 - I)
            // {
            //     V[0xF] = 1; // Set carry flag
            // }
        }

        else if ((nibbles.third | nibbles.fourth) == 0x29) // 0xFX29; Load font character hexadecimal from VX into I; may need additional work done
        {
            I = (V[nibbles.sec >> 8] * 0x5) + FONT_LOCATION;
        }

        else if ((nibbles.third | nibbles.fourth) == 0x33) // 0xFX33
        {
            // VX = 0dXYZ
            memory.at(I) = V[nibbles.sec >> 8] / 100;              // Grab X
            memory.at(I + 1) = ((V[nibbles.sec >> 8]) / 10) % 10;  // Grab Y
            memory.at(I + 2) = ((V[nibbles.sec >> 8]) % 100) % 10; // Grab Z
        }

        else if ((nibbles.third | nibbles.fourth) == 0x55)
        {
            int targetRegister = nibbles.sec >> 8;

            for (int i = 0; i <= targetRegister; i++)
            {
                memory.at(I + i) = V[i];
            }
        }

        else if ((nibbles.third | nibbles.fourth) == 0x65)
        {
            int targetRegister = nibbles.sec >> 8;

            for (int i = 0; i <= targetRegister; i++)
            {
                V[i] = memory.at(I + i);
            }
        }

        break;
    }

    default:
        std::cout << "Error! Opcode " << std::hex << (nibbles.first | nibbles.sec | nibbles.third | nibbles.fourth) << " not implemented.\n";
        break;
    }
}

bool CPU::updateScreen()
{
    if (drawFlag)
        drawFlag = false;
    return true;

    return false;
}

int CPU::getDelayTimer()
{
    return delayTimer;
}

int CPU::getSoundTimer()
{
    return soundTimer;
}

void CPU::decrementDelayTimer()
{
    if (delayTimer > 0)
        delayTimer--;
}

void CPU::decrementSoundTimer()
{
    if (soundTimer > 0)
        soundTimer--;
}
