#include "Display.h"

Display::Display()
{
    mWindow = NULL;
    mRenderer = NULL;
}

bool Display::initDisplay()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    mWindow = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);

    if (mWindow == NULL)
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create texture for frame buffer
    mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, SCREEN_HEIGHT, SCREEN_WIDTH);

    return true;
}

void Display::drawScreen(GPU &gpu)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) // Traverse each row
    {
        for (int x = 0; x < SCREEN_WIDTH; x++) // Traverse each column
        {
            uint8_t pixel = gpu.getPixel(x, y);
        }
    }
}

void Display::handleEvents()
{
}