#pragma once

#include "GPU.h" // For constants
#include "imgui.h"
#include "imgui_impl_sdl2.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

class Display {
public:
  // Constants
  static constexpr int BASE_HEIGHT = 32;
  static constexpr int BASE_WIDTH = 64;

  static constexpr int SCREEN_MULITPLIER = 15;
  static constexpr int SCREEN_HEIGHT = BASE_HEIGHT * SCREEN_MULITPLIER;
  static constexpr int SCREEN_WIDTH = BASE_WIDTH * SCREEN_MULITPLIER;

public:
  Display();
  ~Display();

  void drawScreen(GPU &gpu);
  const SDL_Event getEvent();
  bool mRenderImGui;

private:
  void initDisplay();

private:
  SDL_Window *mWindow;     // Window to render to
  SDL_Renderer *mRenderer; // Render target to draw to
  SDL_Texture *mTexture;   // Texture to send to render
  // SDL_GLContext mContext;

  uint32_t mPixels[BASE_HEIGHT *
                   BASE_WIDTH]; // Representation of internal pixel grid

  ImGuiIO* mIo;
};
