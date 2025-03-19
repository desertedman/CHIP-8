#pragma once

#include <iostream>
#include <array>
#include <cstdint> // Include for uint8_t

class CHIP8
{
private:
    std::array<uint8_t, 4096> mMemory; // 4Kb of memory
    std::streamsize mFileSize;

public:
    bool loadRom(const std::string &path);
    void printMemory(int bytes = 0);
};

std::streamsize getFileSize(std::ifstream &inFS);