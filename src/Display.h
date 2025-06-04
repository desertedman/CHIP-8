#pragma once

#include "CPU.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL_rect.h>
#include <memory>

class Chip8;

class Display {
public:
  // Constants
  static constexpr int BASE_HEIGHT = 32;
  static constexpr int BASE_WIDTH = 64;

  static constexpr int SCREEN_MULITPLIER = 15;

public:
  Display(std::shared_ptr<Chip8> &inputChip8Ptr);
  ~Display();

  void drawScreen(std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels);
  const SDL_Event getEvent();
  void calculateResolution();

  std::shared_ptr<Chip8> mChip8Ptr;
  bool mRenderImGui;

  // static int mScreenWidth;
  // static int mScreenHeight;

private:
  void showOpenFileButton();

  SDL_Window *mWindow;     // Window to render to
  SDL_Renderer *mRenderer; // Render target to draw to
  SDL_Texture *mTexture;   // Texture to send to render

  int mWindowWidth;
  int mWindowHeight;
  SDL_Rect mDrawRect;

  uint32_t mPixelColor[BASE_HEIGHT *
                   BASE_WIDTH]; // Representation of internal pixel grid

  ImGuiIO *mIo;
};
