#pragma once

#include "CPU.h"
#include "Display.h"
#include "GPU.h"
#include "Rom.h"

#include <array>
#include <cstdint> // Include for uint8_t
#include <iostream>

class Chip8 {
public:
  Chip8();

  void loadRom(Rom &rom);
  void runEngine();
  void drawToScreen();

  // Debug functions
  void printMemory(int bytes = 0);
  void testEngine();
  void fillScreen();
  void drawToTerminal();

private:
  // Constants
  const int TARGET_INSTRUCTIONS_PER_SECOND = 560;
  const int FREQUENCY = 60;

private:
  bool initialize();

  void testCycleCPU(uint16_t opcode);
  void cycleCPU();
  void handleInput(SDL_Event &e); // Handle input and send to CPU

private:
  std::array<uint8_t, CPU::MEMORY_SIZE> mMemory; // 4Kb of memory
  std::streamsize mFileSize;
  GPU mGPU;
  CPU mCPU;
  Display mDisplay;

  int mInstructionsPerFrame;
  bool running;
};
