#pragma once

#include "GPU.h"
#include "CPU.h"
#include "Display.h"

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
    std::array<uint8_t, MEMORY> mMemory; // 4Kb of memory
    std::streamsize mFileSize;           // Remove later
    GPU mGPU;
    CPU mCPU;
    Display mDisplay;

    int mInstructionsPerFrame;

    void testCycleCPU(uint16_t opcode);
    void cycleCPU();
    void handleInput(); // Handle input and send to CPU

    bool running;

public:
    bool initialize();
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);

    void testEngine();
    void runEngine();

    void fillScreen();     // Debug function. Remove or put into private later
    void drawToTerminal(); // Debug function. Remove or put into private later
    void drawToScreen();
};

std::streamsize getFileSize(std::ifstream &inFS);
