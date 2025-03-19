#pragma once

#include "Graphics.h"
#include "Cpu.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

class Chip8
{
private:
    std::array<uint8_t, MEMORY> mMemory; // 4Kb of memory
    std::streamsize mFileSize;
    GPU mGPU;
    CPU mCPU;

public:
    Chip8();
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);
    void cycleCpu();

    void drawScreen();
};

std::streamsize getFileSize(std::ifstream &inFS);