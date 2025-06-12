#pragma once

#include "imgui.h"

#include <SDL.h>
#include <SDL_rect.h>
#include <SDL_video.h>
#include <memory>

#include "Chip8.h"

class Chip8;

class Display {
public:
  Display();
  ~Display();
  void drawScreen(Chip8 &chip8);
  void calculateResolution();

  // Constants
  static constexpr int BASE_HEIGHT = 32;
  static constexpr int BASE_WIDTH = 64;
  static constexpr int SCREEN_MULITPLIER = 15;

  std::shared_ptr<Chip8> mChip8Ptr;
  bool mRenderImGui;
  uint32_t mPixelColor[BASE_HEIGHT *
                       BASE_WIDTH]; // Representation of internal pixel grid

private:
  SDL_Window *mWindow;     // Window to render to
  SDL_Renderer *mRenderer; // Render target to draw to
  SDL_Texture *mTexture;   // Texture to send to render
  SDL_Rect mDrawRect;
  int mWindowWidth;
  int mWindowHeight;
  ImGuiIO *mIo;
};
