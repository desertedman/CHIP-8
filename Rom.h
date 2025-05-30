#pragma once

#include "src/Chip8.h"
#include <fstream>

class Rom {
private:
  std::ifstream mInFS;

public:
  static constexpr int ROM_FILE_SIZE = 3584;

  Rom(const std::string &path);
  ~Rom();
  bool open(const std::string &path);
};

namespace RomFunctions {
  std::streamsize getFileSize(std::ifstream &inFS);
}
