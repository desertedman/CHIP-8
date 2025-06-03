#include "Display.h"
#include "Chip8.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL2/SDL_video.h>
#include <stdexcept>
#include <string>

Display::Display(Chip8 *chip8Ptr) : chip8(chip8Ptr) {
  mRenderImGui = true;

  try {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      throw std::runtime_error(
          std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError() +
          "\n");
    }

    // Create window
    mWindow = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    // Create renderer
    mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
    SDL_RenderSetLogicalSize(
        mRenderer, SCREEN_WIDTH,
        SCREEN_HEIGHT); // Render canvas with SCREEN resolution

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
  (void)mIo;
  mIo->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup style
  ImGui::StyleColorsDark();

  // Setup backends
  ImGui_ImplSDL2_InitForSDLRenderer(mWindow, mRenderer);
  ImGui_ImplSDLRenderer2_Init(mRenderer);
}

Display::~Display() {
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

// This function is not used anymore!
void Display::initDisplay() {

  // Throw an error so I don't use this
  throw std::runtime_error("Stop using initDisplay!\n");

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error(
        std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError() +
        "\n");
  }

  // Create window
  mWindow = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                             SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  // Create renderer
  mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
  SDL_RenderSetLogicalSize(
      mRenderer, SCREEN_WIDTH,
      SCREEN_HEIGHT); // Render canvas with SCREEN resolution

  // Ensure that window and renderer were initialized properly
  if (mWindow == NULL || mRenderer == NULL) {
    throw std::runtime_error(
        std::string("Window could not be created! SDL_Error: ") +
        SDL_GetError() + "\n");
  }

  // Create texture for frame buffer
  mTexture = SDL_CreateTexture(
      mRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
      BASE_WIDTH, BASE_HEIGHT); // Internal texture with BASE resolution
}

void Display::drawScreen(GPU &gpu) {
  int mPixelsItt = 0;                   // Iterator to travel mPixels array
  for (int y = 0; y < BASE_HEIGHT; y++) // Traverse each row
  {
    for (int x = 0; x < BASE_WIDTH; x++) // Traverse each column
    {
      bool pixel = gpu.getPixel(x, y);

      // Internally, bool is stored as 0x1 or 0x0; Multiply by 0xFFFFFFFF to
      // determine if pixel is colored or not
      mPixels[mPixelsItt] = 0xFFFFFFFF * pixel;
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
      chip8->toggleGUI();
    }
    ImGui::SameLine();
    ImGui::Text("(B)");

    if (ImGui::Button("Pause")) {
      chip8->resetEngine();
    }
    ImGui::SameLine();
    ImGui::Text("(Space)");

    if (ImGui::Button("Reset emulator")) {
      chip8->resetEngine();
    }
    ImGui::SameLine();
    ImGui::Text("(Enter)");

    ImGui::End();
  }
  ImGui::Render();

  // Update screen
  SDL_UpdateTexture(mTexture, NULL, mPixels, BASE_WIDTH * sizeof(uint32_t));

  // Clear screen and render
  SDL_RenderClear(mRenderer);
  SDL_RenderCopy(mRenderer, mTexture, NULL, NULL);
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), mRenderer);
  SDL_RenderPresent(mRenderer);
}
