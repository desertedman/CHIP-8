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
    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT); // Render canvas with SCREEN resolution

    // Create texture for frame buffer
    mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, BASE_WIDTH, BASE_HEIGHT); // Internal texture with BASE resolution

    return true;
}

void Display::drawScreen(GPU &gpu)
{
    int mPixelsItt = 0;                   // Iterator to travel mPixels array
    for (int y = 0; y < BASE_HEIGHT; y++) // Traverse each row
    {
        for (int x = 0; x < BASE_WIDTH; x++) // Traverse each column
        {
            uint8_t pixel = gpu.getPixel(x, y);

            mPixels[mPixelsItt] = (0x00FFFFFF * pixel) | 0xFF000000; // TODO: Document this
            mPixelsItt++;
        }
    }

    // Update screen
    SDL_UpdateTexture(mTexture, NULL, mPixels, 64 * sizeof(uint32_t));

    // Clear screen and render
    SDL_RenderClear(mRenderer);
    SDL_RenderCopy(mRenderer, mTexture, NULL, NULL);
    SDL_RenderPresent(mRenderer);
}

SDL_EventType Display::handleInput(SDL_Event &e)
{
    while (SDL_PollEvent(&e))
    {
        // User requests quit
        if (e.type == SDL_QUIT)
        {
            return SDL_QUIT;
        }
    }

    return SDL_KEYDOWN;
}

bool Display::close()
{
    return false;
}