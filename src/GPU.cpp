#include "GPU.h"

GPU::GPU() { initialize(); }

void GPU::initialize() {
  clearScreen(); // Ensure that screen is cleared on creation. Probably
                 // unnecessary
}

void GPU::clearScreen() {
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      mScreen.at(x).at(y) = false;
    }
  }
}

void GPU::fillScreen() {
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      mScreen.at(x).at(y) = true;
    }
  }
}

// Get an X, Y coordinate
// Ex. Input (64, 32). Note that the internal array is by ROW, COLUMN (or Y, X)
// order. This function converts (Y, X) coordinates to (X, Y) coordinates.
const bool GPU::getPixel(int x, int y) {
  // Traverse y Vector first, then x Vector to get X,Y coord
  if (mScreen.at(y).at(x)) {
    return true;
  }

  return false;
}

void GPU::setPixel(int x, int y, bool status) {
  // Traverse y Vector first, then x Vector to get X,Y coord
  mScreen.at(y).at(x) = status;
}

void GPU::xorPixel(int x, int y, bool status) {
  bool *currPixel = &mScreen.at(y).at(x);

  // XOR operation
  *currPixel ^= status;
}
