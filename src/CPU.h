#pragma once

#include "GPU.h"

#ifndef MEMORY
#define MEMORY 4096
#endif

#include <array>
#include <cstdint> // Include for uint8_t

struct Nibbles {
    // Order of hex bytes from most to least significant
    uint16_t first;
    uint16_t sec;
    uint8_t third;
    uint8_t fourth;
};

class CPU
{
private:
    uint16_t mPC;   // Program counter pointer
    GPU *mGraphics; // Pointer to GPU for communication with CPU
    Nibbles nibbles;

public:
    CPU(GPU &graphics);
    // void loadAddress(uint8_t &address); // Load program counter to a pointer to address
    uint16_t fetchOpcode(const std::array<uint8_t, MEMORY> &ram);
    void decodeOpcode(const uint16_t &opcode);
    void executeOpcode();
};