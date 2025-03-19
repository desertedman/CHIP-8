#pragma once

#ifndef REGISTERS
#define REGISTERS 16
#endif

#include "GPU.h"
#include "CPU.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

class Chip8
{
private:
    std::array<uint8_t, MEMORY> mMemory; // 4Kb of memory
    uint8_t V[REGISTERS];                // Registers
    uint16_t *I;                         // Points at locations in memory
    std::streamsize mFileSize;           // Remove later
    GPU mGPU;
    CPU mCPU;

public:
    void initialize();
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);
    void cycleCPU();

    void fillScreen();
    void drawScreen();
};

std::streamsize getFileSize(std::ifstream &inFS);