#pragma once

#include "GPU.h"
#include "CPU.h"
#include "Display.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

class Chip8
{
private:
    std::array<uint8_t, MEMORY> mMemory; // 4Kb of memory
    std::streamsize mFileSize;           // Remove later
    GPU mGPU;
    CPU mCPU;
    Display mDisplay;

    uint8_t delayTimer;
    uint8_t soundTimer;

    void testCycleCPU(uint16_t opcode);
    void cycleCPU();

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