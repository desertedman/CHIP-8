#pragma once

#include "CPU.h"
#include "Rom.h"

#include <SDL_events.h>
#include <array>
#include <cstdint> // Include for uint8_t
#include <iostream>
#include <sys/types.h>

namespace Constants {
static constexpr int FREQUENCY = 60;
static constexpr int DEFAULT_INSTRUCTIONS_PER_SECOND = 560;
} // namespace Constants

class Display;

class Chip8 {
public:
  Chip8();

  void loadRom(const std::string &path);
  void runEngine();

  void setQuit();
  void calcSpeed();
  void resetSpeed();
  void togglePause();
  bool getPauseStatus();
  void resetEngine();
  void toggleGUI();

  int mTargetInstructionsPerSecond = Constants::DEFAULT_INSTRUCTIONS_PER_SECOND;
  Display *mDisplay;

  // Debug functions
  void printMemory(int bytes = 0);
  // void testEngine();
  // void drawToTerminal();

private:
  void cycleCPU();
  void handleInput(SDL_Event &e); // Handle input and send to CPU
  // void testCycleCPU(uint16_t opcode);

  std::array<uint8_t, CPU::MEMORY_SIZE> mMemory; // 4Kb of memory
  std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> mPixels;
  std::streamsize mFileSize;
  CPU mCPU;
  Rom mRom;

  int mInstructionsPerFrame;
  bool running;
  bool pause;
  bool loaded;
};

namespace Emulator {
void runEmulator();
}

namespace DisplayFunctions {
void showOpenFileButton();

void drawScreen(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels,
    Display *display, Chip8 &chip8);
} // namespace DisplayFunctions
