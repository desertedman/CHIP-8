#pragma once

// #include "CPU.h"

#include <SDL_events.h>
#include <array>
#include <cstdint> // Include for uint8_t
#include <iostream>
#include <sys/types.h>

struct Nibbles {
  uint16_t opcode;

  // Order of hex bytes from most to least significant
  uint16_t first;
  uint16_t sec;
  uint8_t third;
  uint8_t fourth;

  uint8_t lastTwo;
};

namespace Constants {
static constexpr int BASE_WIDTH = 64;
static constexpr int BASE_HEIGHT = 32;
static constexpr int MEMORY_START = 0x200;
} // namespace Constants

class Display;

class Chip8 {
public:
  static constexpr int BASE_FREQUENCY = 60;
  static constexpr int DEFAULT_INSTRUCTIONS_PER_SECOND = 560;
  static constexpr int FONT_LOCATION = 0x050;
  static constexpr int MEMORY_SIZE = 4096;
  static constexpr int STACK_SIZE = 4096;
  static constexpr int REGISTERS = 16;
  static constexpr int NUM_KEYS = 16;

  Chip8();
  void loadRom(const std::string &path);
  void runEngine();
  const std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH>
  getInternalPixels() const;
  // State functions
  void setQuit();
  void calcSpeed();
  void resetSpeed();
  void togglePause();
  bool getPauseStatus();
  void resetEngine();
  void toggleGUI();

  int mTargetInstructionsPerSecond = DEFAULT_INSTRUCTIONS_PER_SECOND;
  Display *mDisplay;

  // Debug functions
  void printMemory(int bytes = 0);

private:
  class CPU {
  public:
    CPU();
    void initialize();

    uint16_t fetchOpcode();
    void decodeOpcode(const uint16_t &opcode);
    void executeOpcode(std::array<uint8_t, Constants::BASE_HEIGHT *
                                               Constants::BASE_WIDTH> &pixels);
    void printOpcodeMissing();

    std::array<uint8_t, MEMORY_SIZE> mMemory; // 4Kb of memory
    Nibbles nibbles;

    uint16_t I;                              // Stores memory addresses
    std::array<uint8_t, REGISTERS> V;        // Registers V0 - VF
    std::array<uint16_t, STACK_SIZE> mStack; // Call stack
    uint8_t mStackptr; // Location of top of stack; Range 0-15. 16 is very top
                       // (after) of stack!
    uint16_t mPC;      // Program counter pointer

    bool mInternalKeys[NUM_KEYS];

    uint8_t delayTimer;
    uint8_t soundTimer;

    bool quirks = true;
    bool drawFlag;
    bool keyWasPressedLF;
    int keyLastPressed;

    // Opcode functions
    void op00E0(std::array<uint8_t, Constants::BASE_HEIGHT *
                                        Constants::BASE_WIDTH> &pixels);
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
    void opDXYN(std::array<uint8_t, Constants::BASE_HEIGHT *
                                        Constants::BASE_WIDTH> &pixels);
    void opEX9E();
    void opEXA1();
    void opFX07();
    void opFX0A();
    void opFX15();
    void opFX18();
    void opFX1E();
    void opFX29();
    void opFX33();
    void opFX55();
    void opFX65();
  };

  void cycleCPU();
  void handleInput(SDL_Event &e); // Handle input and send to CPU

  CPU mCPU;
  std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> mPixels;
  std::streamsize mFileSize;
  int mInstructionsPerFrame;
  bool running;
  bool pause;
  bool loaded;
};

namespace Emulator {
void runEmulator();
}

namespace PixelFunctions {
int calculatePixel(int xCoord, int yCoord);
uint8_t getPixel(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels,
    int xCoord, int yCoord);
void xorPixel(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels,
    int xCoord, int yCoord);
} // namespace PixelFunctions
