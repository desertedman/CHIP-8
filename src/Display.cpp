#include "Display.h"
#include "CPU.h"
#include "Chip8.h"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL2/SDL_video.h>
#include <SDL_render.h>
#include <cstdint>
#include <stdexcept>
#include <string>

// Calculate initial screen resolution
// int Display::mScreenWidth = Display::BASE_WIDTH * Display::SCREEN_MULITPLIER;
// int Display::mScreenHeight = Display::BASE_HEIGHT *
// Display::SCREEN_MULITPLIER;

Display::Display(std::shared_ptr<Chip8> &inputChip8Ptr) {
  mRenderImGui = true;
  mChip8Ptr = inputChip8Ptr;

  mWindowHeight = BASE_HEIGHT * Display::SCREEN_MULITPLIER;
  mWindowWidth = BASE_WIDTH * Display::SCREEN_MULITPLIER;

  calculateResolution();

  try {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      throw std::runtime_error(
          std::string("SDL could not initialize! SDL_Error: ") +
          SDL_GetError() + "\n");
    }

    // Create window
    mWindow = SDL_CreateWindow(
        "CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth,
        mWindowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    // Create renderer
    mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
    // SDL_RenderSetLogicalSize(
    //     mRenderer, mScreenWidth,
    //     mScreenHeight); // Render canvas with SCREEN resolution

    // Create texture for frame buffer
    mTexture = SDL_CreateTexture(
        mRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        BASE_WIDTH, BASE_HEIGHT); // Internal texture with BASE resolution

    // Ensure that everything was initialized properly
    if (mWindow == NULL || mRenderer == NULL) {
      throw std::runtime_error(
          std::string("Window could not be created! SDL_Error: ") +
          SDL_GetError() + "\n");
    }

  }

  catch (const std::runtime_error &e) {
    throw;
  }

  // Initialize Imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  mIo = &ImGui::GetIO();
  // (void)mIo;
  mIo->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup style
  ImGui::StyleColorsDark();

  // Setup backends
  ImGui_ImplSDL2_InitForSDLRenderer(mWindow, mRenderer);
  ImGui_ImplSDLRenderer2_Init(mRenderer);
}

Display::~Display() {
  mChip8Ptr = NULL;

  // Shutdown ImGui
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(mRenderer); // Must destroy renderer before window!
  mRenderer = NULL;

  SDL_DestroyTexture(mTexture);
  mTexture = NULL;

  SDL_DestroyWindow(mWindow);
  mWindow = NULL;

  SDL_Quit();
}

void Display::calculateResolution() {
  // Grab current window resolution
  SDL_GetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);

  // Calculate smallest integer scale that can cleanly fit within window size
  int scale = std::min(mWindowWidth / BASE_WIDTH, mWindowHeight / BASE_HEIGHT);

  int renderWidth = BASE_WIDTH * scale;
  int renderHeight = BASE_HEIGHT * scale;

  if (renderWidth > mWindowWidth) {
    std::cout << "Width greater than actual window\n";
  } else if (renderHeight > mWindowHeight) {
    std::cout << "Height greater than actual window\n";
  }

  mDrawRect.x = 0;
  mDrawRect.y = 0;
  mDrawRect.w = renderWidth;
  mDrawRect.h = renderHeight;
}

void Display::drawScreen(std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels) {
  int mPixelsItt = 0;                   // Iterator to travel mPixels array
  for (int y = 0; y < BASE_HEIGHT; y++) // Traverse each row
  {
    for (int x = 0; x < BASE_WIDTH; x++) // Traverse each column
    {
      uint8_t pixel = PixelFunctions::getPixel(pixels, x, y);

      // Internally, bool is stored as 0x1 or 0x0; Multiply by 0xFFFFFFFF to
      // determine if pixel is colored or not
      mPixelColor[mPixelsItt] = 0xFFFFFFFF * pixel;
      mPixelsItt++;
    }
  }

  // Start ImGui frame
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // Draw to ImGui frame
  if (mRenderImGui) {
    // ImGui::ShowDemoWindow(&mRenderImGui);
    ImGui::Begin("CHIP-8 Menu");

    if (ImGui::Button("Toggle GUI")) {
      mChip8Ptr->toggleGUI();
    }
    ImGui::SameLine();
    ImGui::Text("(B)");

    {
      // Construct pauseString based on pauseStatus
      std::string pauseString;

      if (mChip8Ptr->getPauseStatus() == false) {
        pauseString = "Pause";
      } else {
        pauseString = "Unpause";
      }

      if (ImGui::Button(pauseString.c_str())) {
        mChip8Ptr->togglePause();
      }
    }
    ImGui::SameLine();
    ImGui::Text("(Space)");

    if (ImGui::Button("Reset emulator")) {
      mChip8Ptr->resetEngine();
    }
    ImGui::SameLine();
    ImGui::Text("(Enter)");

    // Configure emulation speed
    ImGui::Text("Instructions per second (Speed)");
    ImGui::SliderInt("##speedslider",
                     &(mChip8Ptr->mTargetInstructionsPerSecond), 300, 1100);
    if (ImGui::Button("Set speed")) {
      mChip8Ptr->calcSpeed();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset speed")) {
      mChip8Ptr->resetSpeed();
      mChip8Ptr->calcSpeed();
    }

    showOpenFileButton();

    if (ImGui::Button("Quit")) {
      mChip8Ptr->setQuit();
    }
    ImGui::SameLine();
    ImGui::Text("(ESC)");

    ImGui::Text("Internal Resolution: %d x %d", mDrawRect.w, mDrawRect.h);

    ImGui::End();
  }
  ImGui::Render();
  // SDL_RenderSetScale(mRenderer, mIo->DisplayFramebufferScale.x,
  // mIo->DisplayFramebufferScale.y);

  // Update screen
  SDL_UpdateTexture(mTexture, NULL, mPixelColor, BASE_WIDTH * sizeof(uint32_t));

  // Clear screen and render
  SDL_RenderSetLogicalSize(mRenderer, mWindowWidth, mWindowHeight);
  SDL_SetRenderDrawColor(mRenderer, 128, 128, 128, 255);
  SDL_RenderClear(mRenderer);
  SDL_RenderCopy(mRenderer, mTexture, NULL, &mDrawRect);
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), mRenderer);
  SDL_RenderPresent(mRenderer);
}

void Display::showOpenFileButton() {
  // open Dialog Simple
  if (ImGui::Button("Open ROM")) {
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseRom", "Choose a ROM...",
                                            ".rom,.ch8", config);
  }
  // display
  if (ImGuiFileDialog::Instance()->Display("ChooseRom")) {
    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::cout << "Path: " << filePathName << std::endl;
      mChip8Ptr->loadRom(filePathName);
    }

    // close
    ImGuiFileDialog::Instance()->Close();
  }
}

