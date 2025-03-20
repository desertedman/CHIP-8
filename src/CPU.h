#pragma once

#include "GPU.h"

#ifndef MEMORY
#define MEMORY 4096
#endif

#ifndef STACKSIZE
#define STACKSIZE 16
#endif

#ifndef REGISTERS
#define REGISTERS 16
#endif

#include <array>
#include <cstdint> // Include for uint8_t

struct Nibbles
{
    // Order of hex bytes from most to least significant
    uint16_t first;
    uint16_t sec;
    uint8_t third;
    uint8_t fourth;
};

class CPU
{
private:
    uint16_t mPC;                           // Program counter pointer
    std::array<uint16_t, STACKSIZE> mStack; // Call stack
    uint8_t mStackptr;                      // Location of top of stack; Range 0-15. 16 is very top (after) of stack!
    Nibbles nibbles;

    uint16_t I;           // Stores memory addresses
    uint8_t V[REGISTERS]; // Registers V0 - VF

    bool drawFlag;

public:
    // CPU(std::array<uint8_t, MEMORY> &memory);
    void initialize(uint8_t fontLocation);
    uint16_t fetchOpcode(const std::array<uint8_t, MEMORY> &memory);
    void decodeOpcode(const uint16_t &opcode);
    void executeOpcode(GPU &gpu, std::array<uint8_t, MEMORY> &memory);
    bool updateScreen(); // Responds if screen has been updated since last draw
};