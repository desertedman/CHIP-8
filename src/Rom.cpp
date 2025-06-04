#include "Rom.h"

#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>

// Load and open a Rom
Rom::~Rom() {
  // Automatically close file on destruction
  mFile.close();
}

void Rom::openFile(const std::string &path) {
  // Check if a file is already open
  if (mFile.is_open()) {
    mFile.close();
  }

  // Open file in binary mode; otherwise certain chars may be skipped
  mFile.open(path, std::ios::binary | std::ios::ate);

  if (!mFile) {
    throw std::runtime_error("File at " + path + " failed to open\n");
  }

  // Check file size
  mFileSize = RomFunctions::getFileSize(mFile);
  if (mFileSize > ROM_FILE_SIZE) {
    throw std::runtime_error("File is too large! Must be smaller than " +
                             std::to_string(ROM_FILE_SIZE) + " bytes!\n");
  }

  else if (mFileSize == -1) {
    throw std::runtime_error("Failed to get file size");
  }

  else {
    std::cout << "File is good; " << mFileSize << " bytes.\n";
  }
}

std::streamsize Rom::getSize() const { return mFileSize; }

std::streamsize RomFunctions::getFileSize(std::ifstream &inFS) {
  // Get current position (at end)
  std::streamsize fileSize = inFS.tellg();

  if (fileSize == -1) {
    std::cerr << "Error: Failed to get file size." << std::endl;
    return 0;
  }

  // Move read position back to start of file
  inFS.seekg(0, std::ios::beg);

  return fileSize;
}
