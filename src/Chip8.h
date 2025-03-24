#pragma once

#ifndef REGISTERS
#define REGISTERS 16
#endif

#include "GPU.h"
#include "CPU.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

// Constants
const int UPDATESPERSEC = 60;
const int INSTRUCTIONSPERUPDATE = 9;
const int INSTRUCTIONSPERSEC = UPDATESPERSEC * INSTRUCTIONSPERUPDATE;

class Chip8
{
private:
    std::array<uint8_t, MEMORY> mMemory; // 4Kb of memory
    uint8_t V[REGISTERS];                // Registers
    uint16_t *I;                         // Points at locations in memory
    std::streamsize mFileSize;           // Remove later
    GPU mGPU;
    CPU mCPU;
    Display mDisplay;

    uint8_t delayTimer;
    uint8_t soundTimer;

    clock_t prevTime;

    void testCycleCPU(uint16_t opcode);
    void cycleCPU();
    void handleInput();             // Handle input and send to CPU
    int calculateNumInstructions(); // Calculate number of instructions to execute since last timesnap

    bool running;

public:
    Chip8();
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);
    void cycleCPU();

    void drawScreen();
};

std::streamsize getFileSize(std::ifstream &inFS);