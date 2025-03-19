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
    mGPU.fillScreen();                   // Fill screen for debug purposes
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

void Chip8::cycleCpu()
{
    // Fetch opcode
    uint16_t opcode = mCPU.fetchOpcode(mMemory);

    uint16_t testOpcode = 0x00E0;
    // Decode opcode
    mCPU.decodeOpcode(testOpcode);
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
