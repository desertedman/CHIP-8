#pragma once

#include "GPU.h"

#include <array>
#include <cstdint> // Include for uint8_t

struct Nibbles {
  uint16_t opcode;

  // Order of hex bytes from most to least significant
  uint16_t first;
  uint16_t sec;
  uint8_t third;
  uint8_t fourth;

  uint8_t lastTwo;
};

class CPU {
public:
  // Constants
  static constexpr int FONT_LOCATION = 0x050;
  static constexpr int MEMORY_SIZE = 4096;
  static constexpr int STACK_SIZE = 4096;
  static constexpr int REGISTERS = 16;
  static constexpr int NUM_KEYS = 16;

public:
  CPU();
  void initialize();

  bool mInternalKeys[NUM_KEYS];

  uint16_t fetchOpcode(const std::array<uint8_t, MEMORY_SIZE> &memory);
  void decodeOpcode(const uint16_t &opcode);
  void executeOpcode(GPU &gpu, std::array<uint8_t, MEMORY_SIZE> &memory);
  bool updateScreen(); // Responds if screen has been updated since last draw

  int getDelayTimer();
  int getSoundTimer();
  void decrementDelayTimer();
  void decrementSoundTimer();

private:
  uint16_t mPC;                            // Program counter pointer
  std::array<uint16_t, STACK_SIZE> mStack; // Call stack
  uint8_t mStackptr; // Location of top of stack; Range 0-15. 16 is very top
                     // (after) of stack!
  Nibbles nibbles;

  uint16_t I;           // Stores memory addresses
  uint8_t V[REGISTERS]; // Registers V0 - VF

  uint8_t delayTimer;
  uint8_t soundTimer;

  bool drawFlag;
  bool keyWasPressedLF;
  int keyLastPressed;

private:
  void printOpcodeMissing();

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
  void opDXYN(GPU &gpu, std::array<uint8_t, MEMORY_SIZE> &memory);
  void opEX9E();
  void opEXA1();
  void opFX07();
  void opFX0A();
  void opFX15();
  void opFX18();
  void opFX1E();
  void opFX29();
  void opFX33(std::array<uint8_t, MEMORY_SIZE> &memory);
  void opFX55(std::array<uint8_t, MEMORY_SIZE> &memory);
  void opFX65(std::array<uint8_t, MEMORY_SIZE> &memory);
};
