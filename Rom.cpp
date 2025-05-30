#include "Rom.h"

#include <ios>
#include <iostream>
#include <stdexcept>

Rom::Rom(const std::string &path) {
  // Open file in binary mode; otherwise certain chars may be skipped
  mInFS.open(path, std::ios::binary | std::ios::ate);

  if (!mInFS) {
    throw std::runtime_error("File at " + path + " failed to open\n");
  }

  // Check file size
  // if (mFileSize > ROM_FILE_SIZE) {
  //
  // }
}

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
