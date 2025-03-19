#pragma once

#include "Graphics.h"

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

class Chip8
{
private:
    std::array<uint8_t, 4096> mMemory; // 4Kb of memory
    uint8_t *mPC;
    std::streamsize mFileSize;


public:
    Chip8();
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);
    void cycle();

    Graphics mGraphics;
};

std::streamsize getFileSize(std::ifstream &inFS);