#pragma once

#include "GPU.h" // For constants

#include <SDL2/SDL.h>
#include <iostream>

const int BASE_HEIGHT = 32;
const int BASE_WIDTH = 64;

const int SCREEN_MULITPLIER = 10; // Resolution scale factor
const int SCREEN_HEIGHT = BASE_HEIGHT * SCREEN_MULITPLIER;
const int SCREEN_WIDTH = BASE_WIDTH * SCREEN_MULITPLIER;

class Display
{
private:
    SDL_Window *mWindow;                        // Window to render to
    SDL_Renderer *mRenderer;                    // Render target to draw to
    SDL_Texture *mTexture;                      // Texture to send to render
    uint32_t mPixels[BASE_HEIGHT * BASE_WIDTH]; // Representation of internal pixel grid
    SDL_Event e;                                // Event handler

public:
    Display();
    bool initDisplay();
    void drawScreen(GPU &gpu);
    const SDL_Event getEvent();
    void close();
};