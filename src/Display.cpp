#include "Display.h"

#include <stdexcept>
#include <string>

Display::Display() {
  mWindow = NULL;
  mRenderer = NULL;
  mTexture = NULL;

  try {
    initDisplay();
  }

  catch (const std::runtime_error &e) {
    throw;
  }
}

Display::~Display() {
  SDL_DestroyRenderer(mRenderer); // Must destroy renderer before window!
  mRenderer = NULL;

  SDL_DestroyTexture(mTexture);
  mTexture = NULL;

  SDL_DestroyWindow(mWindow);
  mWindow = NULL;

  SDL_Quit();
}

void Display::initDisplay() {
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

  if (mWindow == NULL) {
    throw std::runtime_error(
        std::string("Window could not be created! SDL_Error: ") +
        SDL_GetError() + "\n");
  }

  // Create renderer
  mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
  SDL_RenderSetLogicalSize(
      mRenderer, SCREEN_WIDTH,
      SCREEN_HEIGHT); // Render canvas with SCREEN resolution

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

  // Update screen
  SDL_UpdateTexture(mTexture, NULL, mPixels, BASE_WIDTH * sizeof(uint32_t));

  // Clear screen and render
  SDL_RenderClear(mRenderer);
  SDL_RenderCopy(mRenderer, mTexture, NULL, NULL);
  SDL_RenderPresent(mRenderer);
}

const SDL_Event Display::getEvent() { return e; }
