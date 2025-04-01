
#include "CPU.h"
#include <iostream>
#include <random>

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
    keyWasPressedLF = false;
    keyLastPressed = -1;

    delayTimer = 0;
    soundTimer = 0;
}

// Not a fan of how this function modifies the CPU object's nibbles struct
// Future refactorings might change this
uint16_t CPU::fetchOpcode(const std::array<uint8_t, MEMORY> &memory)
{
    // Fetch opcode
    uint16_t opcode = (memory.at(mPC) << 8) | (memory.at(mPC + 1)); // Grab 2 bytes and combine them
    nibbles.opcode = opcode;

    // Increment PC
    mPC += 2;

    return nibbles.opcode;
}

void CPU::decodeOpcode(const uint16_t &opcode)
{
    nibbles.first = opcode & 0xF000;
    nibbles.sec = opcode & 0x0F00;
    nibbles.third = opcode & 0x00F0;
    nibbles.fourth = opcode & 0x000F;

    nibbles.lastTwo = (nibbles.third | nibbles.fourth);

    // printf("%0X %0X %0X %0X\n", nibbles.first, nibbles.sec, nibbles.third, nibbles.fourth);
}

void CPU::executeOpcode(GPU &gpu, std::array<uint8_t, MEMORY> &memory)
{
    switch (nibbles.first)
    {

    case 0x0000:

        switch (nibbles.lastTwo)
        {
        case 0xE0:
            op00E0(gpu);
            break;

        case 0xEE:
            op00EE();
            break;

        default:
            std::cout << "Error! Opcode " << std::hex << nibbles.opcode << " not implemented.\n";
            break;
        }

        break;

    case 0x1000:
        op1NNN();
        break;

    case 0x2000:
        op2NNN();
        break;

    case 0x3000:
        op3XNN();
        break;

    case 0x4000:
        op4XNN();
        break;

    case 0x5000:
        op5XY0();
        break;

    case 0x6000:
        op6XNN();
        break;

    case 0x7000:
        op7XNN();
        break;

    case 0x8000:
        switch (nibbles.fourth)
        {
        case 0x0:
            op8XY0();
            break;

        case 0x1:
            op8XY1();
            break;

        case 0x2:
            op8XY2();
            break;

        case 0x3:
            op8XY3();
            break;

        case 0x4:
            op8XY4();
            break;

        case 0x5:
            op8XY5();
            break;

        case 0x6:
            op8XY6();
            break;

        case 0x7:
            op8XY7();
            break;

        case 0xE:
            op8XYE();
            break;

        default:
            std::cout << "Error! Opcode " << std::hex << nibbles.opcode << " not implemented.\n";
            break;
        }

        break;

    case 0x9000:
        op9XY0();
        break;

    case 0xA000:
        opANNN();
        break;

    case 0xB000:
        opBNNN();
        break;

    case 0xC000:
        opCXNN();
        break;

    case 0xD000:
        opDXYN(gpu, memory);
        break;

    case 0xE000:

        switch (nibbles.lastTwo)
        {
        case 0x9E:
            opEX9E();
            break;

        case 0xA1:
            opEXA1();
            break;

        default:
            std::cout << "Error! Opcode " << std::hex << nibbles.opcode << " not implemented.\n";
            break;
        }

        break;

    case 0xF000:

        switch (nibbles.lastTwo)
        {

        case 0x07:
            opFX07();
            break;

        case 0x0A:
            opFX0A();
            break;

        case 0x15:
            opFX15();
            break;

        case 0x18:
            opFX18();
            break;

        case 0x1E:
            opFX1E();
            break;

        case 0x29:
            opFX29();
            break;

        case 0x33:
            opFX33(memory);
            break;

        case 0x55:
            opFX55(memory);
            break;

        case 0x65:
            opFX65(memory);
            break;

        default:
            std::cout << "Error! Opcode " << std::hex << nibbles.opcode << " not implemented.\n";
            break;
        }

        break;

    default:
        std::cout << "Error! Opcode " << std::hex << nibbles.opcode << " not implemented.\n";
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

// Opcode functions
void CPU::op00E0(GPU &gpu) // Clear screen op
{
    drawFlag = true;

    gpu.clearScreen();
}

void CPU::op00EE() // Return from subroutine
{
    // Pop stack and jump to address
    mStackptr--;
    mPC = mStack.at(mStackptr);
}

void CPU::op1NNN() // Jump instruction; PC jumps to NNN
{
    mPC = (nibbles.sec | nibbles.third | nibbles.fourth);
}

void CPU::op2NNN() // Store current PC on stack, then jump to NNN
{
    mStack.at(mStackptr) = mPC;
    mStackptr++; // Should never be greater than 16!
    if (mStackptr > 16)
    {
        std::cout << "mStackPtr somehow greater than 16!\n";
    }

    mPC = (nibbles.sec | nibbles.third | nibbles.fourth);
}

void CPU::op3XNN() // If VX == NN, skip one instruction
{
    if (V[nibbles.sec >> 8] == (nibbles.third | nibbles.fourth))
    {
        mPC += 2;
    }
}

void CPU::op4XNN() // If VX != NN, skip one instruction
{
    if (V[nibbles.sec >> 8] != (nibbles.third | nibbles.fourth))
    {
        mPC += 2;
    }
}

void CPU::op5XY0() // If VX == VY, skip one instruction
{
    if (V[nibbles.sec >> 8] == V[nibbles.third >> 4])
    {
        mPC += 2;
    }
}

void CPU::op6XNN() // VX = NN
{
    V[nibbles.sec >> 8] = (nibbles.third | nibbles.fourth);
}

void CPU::op7XNN() // VX += NN
{
    V[nibbles.sec >> 8] += (nibbles.third | nibbles.fourth);
}

void CPU::op8XY0() // VX = VY
{
    V[nibbles.sec >> 8] = V[nibbles.third >> 4];
}

void CPU::op8XY1() // VX = VX | VY
{
    V[nibbles.sec >> 8] |= V[nibbles.third >> 4];
    V[0xF] = 0;
}

void CPU::op8XY2() // VX = VX & VY
{
    V[nibbles.sec >> 8] &= V[nibbles.third >> 4];
    V[0xF] = 0;
}

void CPU::op8XY3() // VX = VX ^ VY
{
    V[nibbles.sec >> 8] ^= V[nibbles.third >> 4];
    V[0xF] = 0;
}

void CPU::op8XY4() // VX = VX + VY
{
    // If VX + VY > 0xFF (max value for unsigned 8bit), then set carry flag
    // Adjust equation to: VX > 0xFF - VY
    bool carryFlag = V[nibbles.sec >> 8] > 0xFF - V[nibbles.third >> 4];

    V[nibbles.sec >> 8] += V[nibbles.third >> 4];

    if (carryFlag)
    {
        V[0xF] = 1; // Set carry flag
    }

    else
    {
        V[0xF] = 0;
    }
}

void CPU::op8XY5() // VX = VX - VY
{
    // VX - VY < 0 - underflow; need to borrow from VF
    // VX < VY
    bool carryFlag = V[nibbles.sec >> 8] < V[nibbles.third >> 4];

    V[nibbles.sec >> 8] -= V[nibbles.third >> 4];

    // VX < VY; borrow occurred
    if (carryFlag)
    {
        V[0xF] = 0;
    }

    // VX > VY; no borrow
    else
    {
        V[0xF] = 1;
    }
}

void CPU::op8XY6() // Bit shift right
{
    // Modern behavior; VX = VY
    V[nibbles.sec >> 8] = V[nibbles.third >> 4];

    // Requires a temp variable in the edge case of 8FF6
    // If we set VF to corresponding bit first, then we have just overwritten VF!
    uint8_t carryBit = V[nibbles.sec >> 8] & 0b1; // Store last bit into temp variable
    V[nibbles.sec >> 8] >>= 1;                    // Bitshift right in place

    V[0xF] = carryBit; // Set register to last bit
}

void CPU::op8XY7() // VX = VY - VX
{
    // VY - VX < 0 - underflow; need to borrow from VF
    // VY < VX
    bool carryFlag = V[nibbles.third >> 4] < V[nibbles.sec >> 8];

    V[nibbles.sec >> 8] = V[nibbles.third >> 4] - V[nibbles.sec >> 8];

    // VY < VX; borrow occurred
    if (carryFlag)
    {
        V[0xF] = 0;
    }

    // VY > VX; no borrow
    else
    {
        V[0xF] = 1;
    }
}

void CPU::op8XYE() // Bit shift left
{
    // Modern behavior; VX = VY
    V[nibbles.sec >> 8] = V[nibbles.third >> 4];

    // Requires a temp variable in the edge case of 8FFE
    // If we set VF to corresponding bit first, then we have just overwritten VF!
    uint8_t carryBit = V[nibbles.sec >> 8] >> 7; // Store first bit into temp variable
    V[nibbles.sec >> 8] <<= 1;                   // Bitshift left in place

    V[0xF] = carryBit; // Set register to first bit
}

void CPU::op9XY0() // If VX != VY, skip
{
    if (V[nibbles.sec >> 8] != V[nibbles.third >> 4])
    {
        mPC += 2;
    }
}

void CPU::opANNN() // Set index register I to 0xNNN
{
    I = (nibbles.sec | nibbles.third | nibbles.fourth);
}

void CPU::opBNNN() // PC = NNN + V0
{
    mPC = (nibbles.sec | nibbles.third | nibbles.fourth) + V[0];
}

void CPU::opCXNN() // Set VX to a random number, AND'd with NN
{
    V[nibbles.sec >> 8] = (rand() % (0xFF + 1)) & (nibbles.third | nibbles.fourth);
}

// Refer to:
// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font
// https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
void CPU::opDXYN(GPU &gpu, std::array<uint8_t, MEMORY> &memory)
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
}

void CPU::opEX9E() // Check if key in VX is pressed or not. If so, skip an instruction
{
    if (mInternalKeys[V[nibbles.sec >> 8]] == true)
    {
        // std::cout << "Key detected\n";
        mPC += 2;
    }
}

void CPU::opEXA1() // Check if key in VX is pressed or not. If not, skip an instruction
{
    if (mInternalKeys[V[nibbles.sec >> 8]] == false)
    {
        mPC += 2;
    }
}

void CPU::opFX07() // VX = delayTimer
{
    V[nibbles.sec >> 8] = delayTimer;
}

// TODO: Verify this function, it may not work correctly.
// Currently passes Input test, but still unsure.
void CPU::opFX0A() // Wait on any key input. Loop until an input is received
{
    // std::cout << "Waiting on input; 0xFX0A...\n";
    bool anyKeyPressed = false;

    for (int i = 0; i < NUM_KEYS; i++)
    {
        if (mInternalKeys[i] == true)
        {
            anyKeyPressed = true;
            keyWasPressedLF = true;
            keyLastPressed = i;
        }
    }

    // Key was pressed last frame
    if (keyWasPressedLF)
    {
        if (anyKeyPressed) // Loop until key is released!
        {
            mPC -= 2;
        }

        else // Key is released on current frame
        {
            keyWasPressedLF = false;

            // Send hexadecimal value of RELEASED char to VX
            V[nibbles.sec >> 8] = keyLastPressed;
        }
    }

    else if (!anyKeyPressed)
    {
        mPC -= 2; // If no input is received, loop this instruction
    }
}

void CPU::opFX15() // delayTimer = VX
{
    delayTimer = V[nibbles.sec >> 8];
}

void CPU::opFX18() // soundTimer = VX
{
    soundTimer = V[nibbles.sec >> 8];
}

// TODO: Verify behavior of this function
void CPU::opFX1E() // I += VX
{
    I += V[nibbles.sec >> 8];

    // If VX + I > 1000, then set carry flag
    // Adjust equation to: VX > 1000 - I
    // if (V[nibbles.sec >> 8] > 1000 - I)
    // {
    //     V[0xF] = 1; // Set carry flag
    // }
}

void CPU::opFX29() // Load font character hexadecimal from VX into I; may need additional work done
{
    I = (V[nibbles.sec >> 8] * 0x5) + FONT_LOCATION;
}

// TODO: Document this function
void CPU::opFX33(std::array<uint8_t, MEMORY> &memory)
{
    // VX = 0dXYZ
    memory.at(I) = V[nibbles.sec >> 8] / 100;              // Grab X
    memory.at(I + 1) = ((V[nibbles.sec >> 8]) / 10) % 10;  // Grab Y
    memory.at(I + 2) = ((V[nibbles.sec >> 8]) % 100) % 10; // Grab Z
}

// TODO: Document this function
void CPU::opFX55(std::array<uint8_t, MEMORY> &memory)
{
    int targetRegister = nibbles.sec >> 8;

    for (int i = 0; i <= targetRegister; i++)
    {
        memory.at(I + i) = V[i];
    }

    I += targetRegister + 1; // Classic behavior; disable for modern
}

// TODO: Document this function
void CPU::opFX65(std::array<uint8_t, MEMORY> &memory)
{
    int targetRegister = nibbles.sec >> 8;

    for (int i = 0; i <= targetRegister; i++)
    {
        V[i] = memory.at(I + i);
    }

    I += targetRegister + 1; // Classic behavior; disable for modern
}
