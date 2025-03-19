#pragma once

#include "Graphics.h"

#ifndef MEMORY
#define MEMORY 4096
#endif

#include <array>
#include <cstdint> // Include for uint8_t

class CPU
{
private:
    uint16_t mPC;   // Program counter pointer
    GPU *mGraphics; // Pointer to GPU for communication with CPU

public:
    CPU(GPU &graphics);
    // void loadAddress(uint8_t &address); // Load program counter to a pointer to address
    uint16_t fetchOpcode(const std::array<uint8_t, MEMORY> &ram);
    void decodeOpcode(const uint16_t &opcode);
};