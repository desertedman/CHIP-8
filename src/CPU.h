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

#ifndef FONT_LOCATION
#define FONT_LOCATION 0x050
#endif

#ifndef NUM_KEYS
#define NUM_KEYS 16
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

    uint8_t delayTimer;
    uint8_t soundTimer;

    bool drawFlag;
    bool keyWasPressedLF;

private:
    // Opcode functions
    void op00E0(GPU &gpu);
    void op00EE();
    void op1NNN();
    void op2NNN();
    void op3XNN();
    void op4XNN();
    void op5XY0();
    void op6XNN();
    void op7XNN();
    void op8XY0();
    void op8XY1();
    void op8XY2();
    void op8XY3();
    void op8XY4();
    void op8XY5();
    void op8XY6();
    void op8XY7();
    void op8XYE();
    void op9XY0();
    void opANNN();
    void opBNNN();
    void opCXNN();
    void opDXYN(GPU &gpu, std::array<uint8_t, MEMORY> &memory);
    void opEX9E();
    void opEXA1();
    void opFX07();
    void opFX0A();
    void opFX15();
    void opFX18();
    void opFX1E();
    void opFX29();
    void opFX33(std::array<uint8_t, MEMORY> &memory);
    void opFX55(std::array<uint8_t, MEMORY> &memory);
    void opFX65(std::array<uint8_t, MEMORY> &memory);


public:
    bool mInternalKeys[NUM_KEYS];

    // CPU(std::array<uint8_t, MEMORY> &memory);
    void initialize();
    uint16_t fetchOpcode(const std::array<uint8_t, MEMORY> &memory);
    void decodeOpcode(const uint16_t &opcode);
    void executeOpcode(GPU &gpu, std::array<uint8_t, MEMORY> &memory);
    bool updateScreen(); // Responds if screen has been updated since last draw

    int getDelayTimer();
    int getSoundTimer();
    void decrementDelayTimer();
    void decrementSoundTimer();
};