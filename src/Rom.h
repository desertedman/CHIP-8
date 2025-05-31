#pragma once

#include <fstream>

class Rom {
public:
  // Constants
  static constexpr int ROM_FILE_SIZE = 3584;

public:
  Rom(const std::string &path);
  ~Rom();
  void openFile(const std::string &path);
  std::streamsize getSize() const;

public:
  std::ifstream mFile;

private:
  std::streamsize mFileSize;
};

namespace RomFunctions {
std::streamsize getFileSize(std::ifstream &inFS);
}
