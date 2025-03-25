#pragma once

#include "GPU.h"
#include "CPU.h"
#include "Display.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

const double FREQUENCY = 60; // Clocks per second
const int TARGET_INSTRUCTIONS_PER_SECOND = 560;

enum KEYS
{
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_COUNT
};

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

    bool keysPressed[KEY_COUNT];

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
