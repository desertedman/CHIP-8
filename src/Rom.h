#pragma once

#include <fstream>

class Rom {
public:
  // Constants
  static constexpr int ROM_FILE_SIZE = 3584;

  ~Rom();
  void openFile(const std::string &path);
  std::streamsize getSize() const;

  std::ifstream mFile;

private:
  std::streamsize mFileSize;
};

namespace RomFunctions {
std::streamsize getFileSize(std::ifstream &inFS);
}
