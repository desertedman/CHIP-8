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
    // Initialize GPU and CPU; reset to their original states
    mGPU.initialize();
    mCPU.initialize();

    // Load font into memory
    int fontLength = sizeof(Font) / sizeof(uint8_t);
    std::memcpy(&mMemory.at(0x050), Font, fontLength);
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

void Chip8::cycleCPU()
{
    // Fetch opcode
    uint16_t opcode = mCPU.fetchOpcode(mMemory);

    uint16_t testOpcode = 0x00E0; // Clear screen opcode for debug purposes
    mCPU.decodeOpcode(testOpcode);
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
