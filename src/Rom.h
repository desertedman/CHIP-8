#pragma once

#include <fstream>

class Rom {
private:
  std::ifstream mFile;
  std::streamsize mFileSize;

public:
  static constexpr int ROM_FILE_SIZE = 3584;

  Rom(const std::string &path);
  ~Rom();
  void openFile(const std::string &path);
  std::ifstream &getFile();
  std::streamsize getSize() const;
};

namespace RomFunctions {
std::streamsize getFileSize(std::ifstream &inFS);
}
