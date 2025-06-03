#pragma once

#include "CPU.h"
#include "GPU.h"
#include "Rom.h"

#include <SDL_events.h>
#include <array>
#include <cstdint> // Include for uint8_t
#include <iostream>

class Display;

class Chip8 {
public:
  Chip8();

  void loadRom(Rom &rom);
  void runEngine();

  void quitEngine();
  void calcSpeed();
  void resetSpeed();
  void togglePause();
  bool getPauseStatus();
  void resetEngine();
  void toggleGUI();

  int TARGET_INSTRUCTIONS_PER_SECOND = DEFAULT_INSTRUCTIONS_PER_SECOND;
  Display *mDisplay;

  // Debug functions
  void printMemory(int bytes = 0);
  void testEngine();
  void fillScreen();
  void drawToTerminal();

private:
  // Constants
  static constexpr int FREQUENCY = 60;
  static constexpr int DEFAULT_INSTRUCTIONS_PER_SECOND = 560;

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

  int mInstructionsPerFrame;
  bool running;
  bool pause;
};
