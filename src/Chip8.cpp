#ifndef FILESIZE
#define FILESIZE 3584 // 4096 - 512 = 3584
#endif

#include "Chip8.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

Chip8::Chip8() : mCPU(mGPU) // Immediately initialize CPU to point to our GPU
{
    mGPU.fillScreen();        // Fill screen for debug purposes
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
    if (mFileSize > (FILESIZE))
    {
        std::cerr << "File is too large! Must be smaller than " << FILESIZE << " bytes!" << std::endl;

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
    // 3. Repeat according to a timer?

    clock_t delayPrevTime = clock() / CLOCKS_PER_SEC;

    running = true;
    while (running)
    {
        // Get number of times to cycle CPU
        int instructions = calculateNumInstructions();

        // Cycle CPU appropriate number of times
        for (int i = 0; i < instructions; i++)
        {
            cycleCPU();
            if (mCPU.updateScreen())
            {
                drawToScreen();
            }

            handleInput();
        }
    }

    mDisplay.close();
}

void Chip8::handleInput()
{
    SDL_Event e = mDisplay.getEvent();
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
            case SDLK_UP:
                std::cout << "Key pressed\n";
                break;
            }

            break;

        default:
            break;
        }
    }
}

int Chip8::calculateNumInstructions()
{
    // Get time since program launch
    const clock_t currTime = clock() / CLOCKS_PER_SEC;
    clock_t timePassed = currTime - prevTime;
    prevTime = currTime;

    int machineInstructions = timePassed * INSTRUCTIONSPERSEC;

    return machineInstructions;
}

void Chip8::cycleCPU()
{
    // Fetch opcode
    uint16_t opcode = mCPU.fetchOpcode(mMemory);

    uint16_t testOpcode = 0x00E0; // Clear screen opcode for debug purposes
    mCPU.decodeOpcode(testOpcode);
    mCPU.executeOpcode();

}

void Chip8::drawScreen()
{
    mGPU.drawScreen();
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
