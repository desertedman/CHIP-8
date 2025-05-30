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
  bool openFile(const std::string &path);
  const std::ifstream& getFile() const;
  const std::streamsize getSize() const;
};

namespace RomFunctions {
  std::streamsize getFileSize(std::ifstream &inFS);
}
