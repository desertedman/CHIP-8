#include "Chip8.h"
// #include "CPU.h"
#include "Display.h"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#ifdef PLATFORM_LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#elif PLATFORM_WINDOWS
#include <SDL.h>
#include <SDL_render.h>
#endif

#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_video.h>
#include <chrono>
#include <cstdint>
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

uint8_t SDL_KEYS[Chip8::NUM_KEYS]{
    SDLK_x, // Corresponds to mInternalKey[0]
    SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f,
    SDLK_v // Corresponds to mInternalKey[15] (or mInternalKey[F])
};

Chip8::Chip8() {
  pause = false;
  loaded = false;
  mDisplay = NULL;
  running = true;
  mPixels.fill(0);

  // Load font into memory
  int fontLength = sizeof(Font) / sizeof(uint8_t);
  std::memcpy(&mMemory.at(FONT_LOCATION), Font, fontLength);

  // Calculate number of instructions to run in a frame
  calcSpeed();
}

void Chip8::loadRom(const std::string &path) {
  std::cout << "Attempting to load file..." << std::endl;

  mRom.openFile(path);

  // Clear original Rom data
  std::memset(mMemory.data() + Constants::MEMORY_START, 0, Rom::ROM_FILE_SIZE);

  // Read rom data into memory
  mRom.mFile.read(
      reinterpret_cast<char *>(mMemory.data() + Constants::MEMORY_START),
      mRom.getSize());

  // Save file size for debug purposes
  mFileSize = mRom.getSize();

  resetEngine();
  loaded = true;
}

void Chip8::printMemory(int bytes) {
  if (bytes == 0) {
    bytes = mFileSize;
  }

  for (int i = 0; i < bytes; i++) {
    printf("%02X ", mMemory.at(Constants::MEMORY_START + i));
  }

  std::cout << "\n";
}

void Chip8::runEngine() {

  // Calculate basic timing info
  double periodSec = 1.0 / FREQUENCY; // Time in seconds to wait for one frame
  std::chrono::duration<double, std::milli> periodMS(periodSec *
                                                     1000); // Convert to ms
  auto nextTime =
      std::chrono::steady_clock::now() + periodMS; // Get current time

  SDL_Event event;

  // 1. Handle input; should translate SDL events to our CPU
  // 2. Decrement timers
  // 3. Cycle CPU
  // 4. Draw screen (only if drawFlag is set)
  // 5. Sleep til next frame; repeat 60 times per sec
  while (running) {
    // Handle input
    handleInput(event);

    if (pause == false && loaded == true) {
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
    DisplayFunctions::drawScreen(mPixels, mDisplay, *this);

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

    else if (event.type == SDL_WINDOWEVENT &&
             event.window.event == SDL_WINDOWEVENT_RESIZED) {
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
        for (int i = 0; i < NUM_KEYS; i++) {
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
      for (int i = 0; i < NUM_KEYS; i++) {
        if (event.key.keysym.sym == SDL_KEYS[i] &&
            mCPU.mInternalKeys[i] == true) {
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
  mCPU.executeOpcode(mPixels, mMemory);
}

void Chip8::setQuit() {
  running = false;
  std::cout << "Quitting emulator...\n";
}

void Chip8::calcSpeed() {
  mInstructionsPerFrame = mTargetInstructionsPerSecond / FREQUENCY;
}

void Chip8::resetSpeed() {
  mTargetInstructionsPerSecond = DEFAULT_INSTRUCTIONS_PER_SECOND;
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

bool Chip8::getPauseStatus() { return pause; }

void Chip8::resetEngine() {
  mCPU.initialize();
  mPixels.fill(0);
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

void Emulator::runEmulator() {
  try {
    std::shared_ptr<Chip8> chip8 = std::make_shared<Chip8>();
    Display display;
    chip8->mDisplay = &display;

    chip8->runEngine();
  }

  catch (const std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
  }
}

void DisplayFunctions::drawScreen(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels,
    Display *display, Chip8 &chip8) {
  int mPixelsItt = 0; // Iterator to travel mPixels array

  // Traverse each row
  for (int y = 0; y < Constants::BASE_HEIGHT; y++) {
    // Traverse each column
    for (int x = 0; x < Constants::BASE_WIDTH; x++) {
      uint8_t pixel = PixelFunctions::getPixel(pixels, x, y);

      // Internally, bool is stored as 0x1 or 0x0; Multiply by 0xFFFFFFFF to
      // determine if pixel is colored or not
      display->mPixelColor[mPixelsItt] = 0xFFFFFFFF * pixel;
      mPixelsItt++;
    }
  }

  // Start ImGui frame
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // Draw to ImGui frame
  if (display->mRenderImGui) {
    // ImGui::ShowDemoWindow(&mRenderImGui);
    ImGui::Begin("CHIP-8 Menu");

    if (ImGui::Button("Toggle GUI")) {
      chip8.toggleGUI();
    }
    ImGui::SameLine();
    ImGui::Text("(B)");

    {
      // Construct pauseString based on pauseStatus
      std::string pauseString;

      if (chip8.getPauseStatus() == false) {
        pauseString = "Pause";
      } else {
        pauseString = "Unpause";
      }

      if (ImGui::Button(pauseString.c_str())) {
        chip8.togglePause();
      }
    }
    ImGui::SameLine();
    ImGui::Text("(Space)");

    if (ImGui::Button("Reset emulator")) {
      chip8.resetEngine();
    }
    ImGui::SameLine();
    ImGui::Text("(Enter)");

    // Configure emulation speed
    ImGui::Text("Instructions per second (Speed)");
    ImGui::SliderInt("##Speedslider", &(chip8.mTargetInstructionsPerSecond),
                     300, 1100);
    if (ImGui::Button("Set speed")) {
      chip8.calcSpeed();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset speed")) {
      chip8.resetSpeed();
      chip8.calcSpeed();
    }

    {
      // Open ROM button
      if (ImGui::Button("Open ROM")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseRom", "Choose a ROM...",
                                                ".rom,.ch8", config);
      }
      // display
      if (ImGuiFileDialog::Instance()->Display("ChooseRom")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
          std::string filePathName =
              ImGuiFileDialog::Instance()->GetFilePathName();
          std::cout << "Path: " << filePathName << std::endl;
          chip8.loadRom(filePathName);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
      }
    }

    if (ImGui::Button("Quit")) {
      chip8.setQuit();
    }
    ImGui::SameLine();
    ImGui::Text("(ESC)");

    ImGui::Text("Internal resolution: %d x %d", display->mDrawRect.w,
                display->mDrawRect.h);

    {
      // TODO: Remove this section. This is super dangerous, but I just have it
      // for debug purposes
      int width, height;
      SDL_GetWindowSize(display->mWindow, &width, &height);
      ImGui::Text("Window resolution: %d x %d", width, height);
    }

    ImGui::End();
  }
  ImGui::Render();

  // Update screen
  SDL_UpdateTexture(display->mTexture, NULL, display->mPixelColor,
                    Constants::BASE_WIDTH * sizeof(uint32_t));

  // Clear screen and render
  SDL_SetRenderDrawColor(display->mRenderer, 128, 128, 128, 255);
  SDL_RenderClear(display->mRenderer);
  // Resizing window larger does not work properly without this! Why??
  SDL_RenderSetViewport(display->mRenderer, NULL);
  SDL_RenderCopy(display->mRenderer, display->mTexture, NULL,
                 &display->mDrawRect);
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),
                                        display->mRenderer);
  SDL_RenderPresent(display->mRenderer);
}
