#ifndef FILESIZE
#define FILESIZE 3584 // 4096 - 512 = 3584
#endif

#include "CHIP8.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

CHIP8::CHIP8()
{
    mPC = &mMemory.at(0x200); // Start program counter at beginning of ROM
}

bool CHIP8::loadRom(const std::string &path)
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

void CHIP8::printMemory(int bytes)
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

void CHIP8::cycle()
{
    // Fetch opcode
    // uint16_t opcode = (*mPC << 8) | *(mPC + 1); // Grab 2 bytes and combine them
    uint16_t opcode = 0x00E0;

    // Increment PC
    // mPC += 2;

    // Decode opcode
    switch (opcode & 0xF000) // Grab first hex char
    {
    case (0x0000): // Handles all 0x0XXX opcodes
        mGraphics.clearScreen();
        break;
    }
}

std::streamsize getFileSize(std::ifstream &inFS)
{
    std::streamsize fileSize = inFS.tellg(); // Get current position (at end)
    inFS.seekg(0, std::ios::beg);            // Move read position back to start of file

    return fileSize;
}
