#include "Display.h"
#include "Chip8.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL2/SDL_video.h>
#include <SDL_render.h>
#include <stdexcept>
#include <string>

// Calculate initial screen resolution
// int Display::mScreenWidth = Display::BASE_WIDTH * Display::SCREEN_MULITPLIER;
// int Display::mScreenHeight = Display::BASE_HEIGHT *
// Display::SCREEN_MULITPLIER;

Display::Display() {
  mRenderImGui = true;

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
