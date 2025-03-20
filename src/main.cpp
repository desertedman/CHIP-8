#include "Chip8.h"

#include <SDL2/SDL.h>
#include <iostream>

const int BASE_HEIGHT = 32;
const int BASE_WIDTH = 64;

const int SCREEN_MULITPLIER = 10;

const int SCREEN_HEIGHT = BASE_HEIGHT * SCREEN_MULITPLIER;
const int SCREEN_WIDTH = BASE_WIDTH * SCREEN_MULITPLIER;

int main()
{
    Chip8 chip8;
    chip8.initialize();

    std::cout << "Attempting to load file..." << std::endl;
    std::string Pong = "../roms/Pong (1 player).ch8";
    std::string IBM = "../roms/IBM Logo.ch8";
    if (chip8.loadRom(IBM))
    {
        std::cout << "Successfully opened file!" << std::endl;
    }

    else
    {
        std::cout << "Failed to open file! Closing." << std::endl;
        return -1;
    }

    chip8.runEngine();

    // // Window initialization
    // SDL_Window *window = NULL;
    // SDL_Surface *screenSurface = NULL;

    // // Initialize SDL
    // if (SDL_Init(SDL_INIT_VIDEO) < 0)
    // {
    //     std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    //     return -1;
    // }

    // // Window creation
    // window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    // if (window == NULL)
    // {
    //     std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    //     return -1;
    // }

    // screenSurface = SDL_GetWindowSurface(window); // Get window surface
    // SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    // SDL_UpdateWindowSurface(window);
    // SDL_Event e;
    // bool quit = false;
    // while (quit == false)
    // {
    //     while (SDL_PollEvent(&e))
    //     {
    //         if (e.type == SDL_QUIT)
    //             quit = true;
    //     }
    // }

    // // Destroy window
    // SDL_DestroyWindow(window);
    // // Quit SDL subsystems
    // SDL_Quit();

    return 0;
}