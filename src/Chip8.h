#pragma once

#include "GPU.h"
#include "CPU.h"

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

    void cycleCPU();

public:
    void initialize();
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);

    void runEngine();

    void fillScreen();
    void drawScreen();
};

std::streamsize getFileSize(std::ifstream &inFS);