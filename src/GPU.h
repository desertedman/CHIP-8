#pragma once

#include <array>

class GPU {
public:
  // Constants
  static constexpr int COLUMNS = 64;
  static constexpr int ROWS = 32;

public:
  GPU();
  void initialize();

  void clearScreen();
  void fillScreen();

public:
  const bool getPixel(int x, int y);
  void setPixel(int x, int y, bool status);
  void xorPixel(int x, int y, bool status);

private:
  std::array<std::array<bool, COLUMNS>, ROWS> mScreen; // 32 rows of 64 columns
};
