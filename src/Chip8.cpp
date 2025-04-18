#ifndef FILE_SIZE
#define FILE_SIZE 3584 // 4096 - 512 = 3584
#endif

#include "Chip8.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <SDL2/SDL.h>
#include <thread>
#include <chrono>

uint8_t Font[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t SDL_KEYS[NUM_KEYS]{
    SDLK_x, // Corresponds to mInternalKey[0]
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v // Corresponds to mInternalKey[15] (or mInternalKey[F])
};

bool Chip8::initialize()
{
    // Load font into memory
    int fontLength = sizeof(Font) / sizeof(uint8_t);
    std::memcpy(&mMemory.at(FONT_LOCATION), Font, fontLength);

    // Initialize GPU and CPU; reset to their original states
    mGPU.initialize();
    mCPU.initialize();
    if (!mDisplay.initDisplay())
    {
        std::cout << "Failed to initialize display!\n";
        return false;
    }

    running = true;

    // Calculate number of instructions to run in a frame
    mInstructionsPerFrame = TARGET_INSTRUCTIONS_PER_SECOND / FREQUENCY;

    return true;
}

bool Chip8::loadRom(const std::string &path)
{
    std::ifstream inFS;

    inFS.open(path, std::ios::binary | std::ios::ate); // Open file in binary mode; otherwise certain chars may be skipped
    if (!inFS)                                         // File failed to open
    {
        std::cout << "File at " << path << " failed to open" << std::endl;
        return false;
    }

    // Check file size
    mFileSize = getFileSize(inFS); // Get file size;
    if (mFileSize > (FILE_SIZE))
    {
        std::cerr << "File is too large! Must be smaller than " << FILE_SIZE << " bytes!" << std::endl;

        return false;
    }

    else
    {
        std::cout << "File is good; " << mFileSize << " bytes." << std::endl;
    }

    // Allocate buffer for ROM
    std::vector<uint8_t> buffer(mFileSize);
    inFS.read(reinterpret_cast<char *>(buffer.data()), mFileSize); // Copy file content into buffer

    if (inFS.fail()) // Check if the file read succeeded
    {
        std::cerr << "Error: Could not read the entire file!" << std::endl;
        return false;
    }

    // Copy ROM into memory
    std::memcpy(mMemory.data() + 0x200, buffer.data(), mFileSize);

    inFS.close();
    return true;
}

void Chip8::printMemory(int bytes)
{
    if (bytes == 0)
    {
        bytes = mFileSize;
    }

    for (int i = 0; i < bytes; i++)
    {
        printf("%02X ", mMemory.at(0x200 + i));
    }

    std::cout << "\n";
}

void Chip8::testEngine()
{
    {
        // Test 00E0
        mGPU.fillScreen();
        drawToTerminal();

        testCycleCPU(0x00E0);
        drawToTerminal();
    }

    {
        // 0x1NNN
        std::cout << "\tTesting 0x1NNN\n";
        testCycleCPU(0x1AAA);
    }

    {
        // 0x2NNN and 0x00EE
        std::cout << "\tTesting 0x2NNN\n";
        testCycleCPU(0x235E);

        std::cout << "\tTesting 0x00EE\n";
        testCycleCPU(0x00EE);
    }

    {
        // 0x6XNN
        std::cout << "\tTesting 0x6XNN\n";
        testCycleCPU(0x634A);
    }

    {
        // 0x7XNN
        std::cout << "\tTesting 0x7XNN\n";
        testCycleCPU(0x7501);
        testCycleCPU(0x75FF);
    }

    {
        // 0xANNN
        std::cout << "\tTesting 0xANNN\n";
        testCycleCPU(0xA501);
    }

    {
        // Fill pixel test
        mGPU.setPixel(63, 31, true);
        std::cout << mGPU.getPixel(63, 31) << std::endl;

        drawToTerminal();

        fillScreen();
        drawToTerminal();
    }
}

void Chip8::runEngine()
{
    // 1. Cycle CPU
    // 2. Draw screen (only if drawFlag is set)
    // 3. Handle input; should translate SDL events to our CPU
    // 3. Repeat 60 times a second (60 Hz)

    double periodSec = 1.0 / FREQUENCY;                                   // Time in seconds to wait for one frame
    std::chrono::duration<double, std::milli> periodMS(periodSec * 1000); // Convert to ms

    auto nextTime = std::chrono::steady_clock::now() + periodMS; // Get current time

    while (running)
    {
        for (int i = 0; i < mInstructionsPerFrame; i++)
        {
            cycleCPU(); // Cycle CPU appropriate number of times
        }

        // Decrement timers
        if (mCPU.getDelayTimer() > 0)
        {
            mCPU.decrementDelayTimer();
        }
        if (mCPU.getSoundTimer() > 0)
        {
            mCPU.decrementSoundTimer();
        }

        if (mCPU.updateScreen())
        {
            drawToScreen();
        }

        handleInput();

        // Sleep method
        std::this_thread::sleep_until(nextTime); // Sleep til next frame
        nextTime += periodMS;
    }

    mDisplay.close();
}

void Chip8::handleInput()
{
    SDL_Event e = mDisplay.getEvent();
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
        }

        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }

            else
            {
                // Loop through all keys and check if pressed
                for (int i = 0; i < NUM_KEYS; i++)
                {
                    if (e.key.keysym.sym == SDL_KEYS[i] && mCPU.mInternalKeys[i] == false)
                    {
                        mCPU.mInternalKeys[i] = true;
                        // std::cout << "Key " << SDL_GetKeyName(e.key.keysym.sym) << " pressed.\n";
                        // std::cout << mCPU.mInternalKeys[i] << std::endl;
                    }
                }
            }
        }

        else if (e.type == SDL_KEYUP)
        {
            // Loop through all keys and check if released
            for (int i = 0; i < NUM_KEYS; i++)
            {
                if (e.key.keysym.sym == SDL_KEYS[i] && mCPU.mInternalKeys[i] == true)
                {
                    mCPU.mInternalKeys[i] = false;
                    // std::cout << "Key " << SDL_GetKeyName(e.key.keysym.sym) << " released.\n";
                    // std::cout << mCPU.mInternalKeys[i] << std::endl;
                }
            }
        }
    }
}

void Chip8::cycleCPU()
{
    uint16_t opcode = mCPU.fetchOpcode(mMemory);
    mCPU.decodeOpcode(opcode);
    // std::cout << "Opcode: " << std::hex << opcode << std::endl;
    mCPU.executeOpcode(mGPU, mMemory);
}

void Chip8::testCycleCPU(uint16_t opcode)
{
    mCPU.decodeOpcode(opcode);
    mCPU.executeOpcode(mGPU, mMemory);
}

void Chip8::fillScreen()
{
    mGPU.fillScreen();
}

void Chip8::drawToTerminal()
{
    // Draw top border
    for (int i = 0; i < COLUMNS; i++)
    {
        std::cout << "_";
    }
    std::cout << std::endl;

    for (int y = 0; y < ROWS; y++)
    {
        std::cout << "| "; // Beginning of screen row

        for (int x = 0; x < COLUMNS; x++)
        {
            if (mGPU.getPixel(x, y))
            {
                std::cout << "*";
            }

            else
            {
                std::cout << " ";
            }
        }

        std::cout << " |" << std::endl; // End of row; start new line;
    }

    // Draw bottom border
    for (int i = 0; i < COLUMNS; i++)
    {
        std::cout << "_";
    }
    std::cout << std::endl;
}

void Chip8::drawToScreen()
{
    mDisplay.drawScreen(mGPU);
}

std::streamsize getFileSize(std::ifstream &inFS)
{
    std::streamsize fileSize = inFS.tellg(); // Get current position (at end)
    if (fileSize == -1)
    {
        std::cerr << "Error: Failed to get file size." << std::endl;
        return 0;
    }

    inFS.seekg(0, std::ios::beg); // Move read position back to start of file

    return fileSize;
}