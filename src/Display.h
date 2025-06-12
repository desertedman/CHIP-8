#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"

#ifdef PLATFORM_LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#elif PLATFORM_WINDOWS
#include "SDL.h"
#include "SDL_video.h"
#endif

#include <SDL_rect.h>
#include <memory>

class Chip8;

class Display {
public:
  // Constants
  static constexpr int BASE_HEIGHT = 32;
  static constexpr int BASE_WIDTH = 64;

  static constexpr int SCREEN_MULITPLIER = 15;

  Display();
  ~Display();

  const SDL_Event getEvent();
  void calculateResolution();

  SDL_Window *mWindow;     // Window to render to
  SDL_Renderer *mRenderer; // Render target to draw to
  SDL_Texture *mTexture;   // Texture to send to render
  SDL_Rect mDrawRect;

  std::shared_ptr<Chip8> mChip8Ptr;
  bool mRenderImGui;
  uint32_t mPixelColor[BASE_HEIGHT *
                       BASE_WIDTH]; // Representation of internal pixel grid

private:
  int mWindowWidth;
  int mWindowHeight;
  ImGuiIO *mIo;
};
