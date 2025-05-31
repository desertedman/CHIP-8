#pragma once

#include "GPU.h"
#include "CPU.h"
#include "Display.h"
#include "Rom.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

#ifndef FREQUENCY
#define FREQUENCY 60
#endif

const int TARGET_INSTRUCTIONS_PER_SECOND = 560;

class Chip8
{
private:
    bool initialize();

    std::array<uint8_t, MEMORY> mMemory; // 4Kb of memory
    std::streamsize mFileSize;
    GPU mGPU;
    CPU mCPU;
    Display mDisplay;

    int mInstructionsPerFrame;

    void testCycleCPU(uint16_t opcode);
    void cycleCPU();
    void handleInput(); // Handle input and send to CPU

    bool running;

public:
    Chip8();
    ~Chip8();

    void loadRom(Rom &rom);
    void runEngine();
    void drawToScreen();

    // Debug functions
    void printMemory(int bytes = 0);
    void testEngine();
    void fillScreen();
    void drawToTerminal();
};

std::streamsize getFileSize(std::ifstream &inFS);
