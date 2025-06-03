#include "Chip8.h"
#include "Display.h"
#include "imgui_impl_sdl2.h"

#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_video.h>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

uint8_t Font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t SDL_KEYS[CPU::NUM_KEYS]{
    SDLK_x, // Corresponds to mInternalKey[0]
    SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f,
    SDLK_v // Corresponds to mInternalKey[15] (or mInternalKey[F])
};

Chip8::Chip8() {
  pause = false;

  if (!initialize()) {
    std::cerr << "Failed to initialize engine!\n";
    exit(-1);
  }
}

bool Chip8::initialize() {
  // Load font into memory
  int fontLength = sizeof(Font) / sizeof(uint8_t);
  std::memcpy(&mMemory.at(CPU::FONT_LOCATION), Font, fontLength);

  // Initialize GPU and CPU; reset to their original states
  // mGPU.initialize();
  // mCPU.initialize();
  // Ideally, should not need to initialize CPU and GPU manually

  running = true;

  // Calculate number of instructions to run in a frame
  calcSpeed();

  return true;
}

void Chip8::loadRom(Rom &rom) {
  // Read rom data into memory
  rom.mFile.read(reinterpret_cast<char *>(mMemory.data() + 0x200),
                 rom.getSize());

  // Save file size for debug purposes
  mFileSize = rom.getSize();
}

void Chip8::printMemory(int bytes) {
  if (bytes == 0) {
    bytes = mFileSize;
  }

  for (int i = 0; i < bytes; i++) {
    printf("%02X ", mMemory.at(0x200 + i));
  }

  std::cout << "\n";
}

void Chip8::testEngine() {
  {
    // Test 00E0
    mGPU.fillScreen();
    drawToTerminal();

    testCycleCPU(0x00E0);
    drawToTerminal();
  }

  {
    // 0x1NNN
    std::cout << "\tTesting 0x1NNN\n";
    testCycleCPU(0x1AAA);
  }

  {
    // 0x2NNN and 0x00EE
    std::cout << "\tTesting 0x2NNN\n";
    testCycleCPU(0x235E);

    std::cout << "\tTesting 0x00EE\n";
    testCycleCPU(0x00EE);
  }

  {
    // 0x6XNN
    std::cout << "\tTesting 0x6XNN\n";
    testCycleCPU(0x634A);
  }

  {
    // 0x7XNN
    std::cout << "\tTesting 0x7XNN\n";
    testCycleCPU(0x7501);
    testCycleCPU(0x75FF);
  }

  {
    // 0xANNN
    std::cout << "\tTesting 0xANNN\n";
    testCycleCPU(0xA501);
  }

  {
    // Fill pixel test
    mGPU.setPixel(63, 31, true);
    std::cout << mGPU.getPixel(63, 31) << std::endl;

    drawToTerminal();

    fillScreen();
    drawToTerminal();
  }
}

void Chip8::runEngine() {
  // 1. Handle input; should translate SDL events to our CPU
  // 2. Decrement timers
  // 3. Cycle CPU
  // 4. Draw screen (only if drawFlag is set)
  // 5. Sleep til next frame; repeat 60 times per sec

  double periodSec = 1.0 / FREQUENCY; // Time in seconds to wait for one frame
  std::chrono::duration<double, std::milli> periodMS(periodSec *
                                                     1000); // Convert to ms

  auto nextTime =
      std::chrono::steady_clock::now() + periodMS; // Get current time

  SDL_Event event;

  while (running) {
    // Handle input
    handleInput(event);

    if (pause == false) {
      // Decrement timers
      if (mCPU.getDelayTimer() > 0) {
        mCPU.decrementDelayTimer();
      }
      if (mCPU.getSoundTimer() > 0) {
        mCPU.decrementSoundTimer();
      }

      //  TODO: Currently running at 9.33 instructions for frame; we are losing
      //  one instruction every 3 frames!

      // Execute instructions per frame
      for (int i = 0; i < mInstructionsPerFrame; i++) {
        cycleCPU(); // Cycle CPU appropriate number of times
      }
    }

    // Draw screen outside of pause loop so that ImGui still updates
    mDisplay->drawScreen(mGPU);

    // Sleep method
    std::this_thread::sleep_until(nextTime); // Sleep til next frame
    nextTime += periodMS;
  }
}

void Chip8::handleInput(SDL_Event &event) {
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);

    if (event.type == SDL_QUIT) {
      setQuit();
    }

    else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
      Display::mScreenWidth = event.window.data1;
      // Display::mScreenHeight = event.window.data2;
      mDisplay->calculateResolution();
    }

    else if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        setQuit();
      }

      else if (event.key.keysym.sym == SDLK_SPACE) {
        togglePause();
      }

      else if (event.key.keysym.sym == SDLK_b) {
        toggleGUI();
      }

      else if (event.key.keysym.sym == SDLK_RETURN) {
        resetEngine();
      }

      else {
        // Loop through all keys and check if pressed
        for (int i = 0; i < CPU::NUM_KEYS; i++) {
          if (event.key.keysym.sym == SDL_KEYS[i] &&
              mCPU.mInternalKeys[i] == false) {
            mCPU.mInternalKeys[i] = true;
            // std::cout << "Key " << SDL_GetKeyName(e.key.keysym.sym) << "
            // pressed.\n"; std::cout << mCPU.mInternalKeys[i] << std::endl;
          }
        }
      }
    }

    else if (event.type == SDL_KEYUP) {
      // Loop through all keys and check if released
      for (int i = 0; i < CPU::NUM_KEYS; i++) {
        if (event.key.keysym.sym == SDL_KEYS[i] && mCPU.mInternalKeys[i] == true) {
          mCPU.mInternalKeys[i] = false;
          // std::cout << "Key " << SDL_GetKeyName(e.key.keysym.sym) << "
          // released.\n"; std::cout << mCPU.mInternalKeys[i] << std::endl;
        }
      }
    }
  }
}

void Chip8::cycleCPU() {
  uint16_t opcode = mCPU.fetchOpcode(mMemory);
  mCPU.decodeOpcode(opcode);
  // std::cout << "Opcode: " << std::hex << opcode << std::endl;
  mCPU.executeOpcode(mGPU, mMemory);
}

void Chip8::testCycleCPU(uint16_t opcode) {
  mCPU.decodeOpcode(opcode);
  mCPU.executeOpcode(mGPU, mMemory);
}

void Chip8::fillScreen() { mGPU.fillScreen(); }

void Chip8::drawToTerminal() {
  // Draw top border
  for (int i = 0; i < GPU::COLUMNS; i++) {
    std::cout << "_";
  }
  std::cout << std::endl;

  for (int y = 0; y < GPU::ROWS; y++) {
    std::cout << "| "; // Beginning of screen row

    for (int x = 0; x < GPU::COLUMNS; x++) {
      if (mGPU.getPixel(x, y)) {
        std::cout << "*";
      }

      else {
        std::cout << " ";
      }
    }

    std::cout << " |" << std::endl; // End of row; start new line;
  }

  // Draw bottom border
  for (int i = 0; i < GPU::COLUMNS; i++) {
    std::cout << "_";
  }
  std::cout << std::endl;
}

void Chip8::setQuit() {
  running = false;
  std::cout << "Quitting emulator...\n";
}

void Chip8::calcSpeed() {
  mInstructionsPerFrame = TARGET_INSTRUCTIONS_PER_SECOND / FREQUENCY;
}

void Chip8::resetSpeed() {
  TARGET_INSTRUCTIONS_PER_SECOND = DEFAULT_INSTRUCTIONS_PER_SECOND;
}


void Chip8::togglePause() {
  if (pause) {
    pause = false;
    std::cout << "Unpaused\n";
  }

  else {
    pause = true;
    std::cout << "Paused\n";
  }
}

bool Chip8::getPauseStatus() {
  return pause;
}

void Chip8::resetEngine() {
  mCPU.initialize();
  mGPU.initialize();
  std::cout << "State has been reset!\n";
}

void Chip8::toggleGUI() {
  if (mDisplay->mRenderImGui == true) {
    mDisplay->mRenderImGui = false;
    std::cout << "Toggling gui off\n";
  }

  else {
    mDisplay->mRenderImGui = true;
    std::cout << "Toggling gui on\n";
  }
}
