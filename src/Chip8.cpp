#ifndef FILESIZE
#define FILESIZE 3584 // 4096 - 512 = 3584
#endif

#include "Chip8.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

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

void Chip8::initialize()
{
    // Load font into memory
    int fontLength = sizeof(Font) / sizeof(uint8_t);
    uint8_t fontLocation = 0x050;
    std::memcpy(&mMemory.at(fontLocation), Font, fontLength);

    // Initialize GPU and CPU; reset to their original states
    mGPU.initialize();
    mCPU.initialize();
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
        mGPU.drawScreen();

        testCycleCPU(0x00E0);
        mGPU.drawScreen();
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

        drawScreen();

        fillScreen();
        drawScreen();
    }
}

void Chip8::runEngine()
{
    // 1. Cycle CPU
    // 2. Draw screen (only if drawFlag is set)
    // 3. Repeat according to a timer?

    while (true)
    {
        cycleCPU();
        if (mCPU.updateScreen())
        {
            mGPU.drawScreen();
        }
    }
}

void Chip8::cycleCPU()
{
    uint16_t opcode = mCPU.fetchOpcode(mMemory);
    mCPU.decodeOpcode(opcode);
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
