#pragma once

#include <fstream>

class Rom
{
  public:
    // Constants
    static constexpr int ROM_FILE_SIZE = 3584;

    void openFile(const std::string &path);
    const std::streamsize getSize() const;

    std::ifstream mFile;

  private:
    std::streamsize mFileSize;
};

namespace RomFunctions
{
std::streamsize getFileSize(std::ifstream &inFS);
}
