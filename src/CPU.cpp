// #include "CPU.h"
#include "Chip8.h"
#include <cstdint>
#include <iostream>

Chip8::CPU::CPU() { initialize(); }

void Chip8::CPU::initialize() {
  // Reset stack
  mPC = Constants::MEMORY_START;
  mStackptr = 0;
  mStack.fill(0);

  // Reset nibbles
  nibbles.first = 0;
  nibbles.sec = 0;
  nibbles.third = 0;
  nibbles.fourth = 0;
  nibbles.opcode = 0;

  // Set memory pointer
  I = 0;

  // Clear registers
  V.fill(0);

  drawFlag = false;
  keyWasPressedLF = false;
  keyLastPressed = -1;

  delayTimer = 0;
  soundTimer = 0;
}

uint16_t Chip8::CPU::fetchOpcode() {
  // Fetch opcode
  uint16_t opcode = (mMemory.at(mPC) << 8) |
                    (mMemory.at(mPC + 1)); // Grab 2 bytes and combine them

  // Increment PC
  mPC += 2;

  return opcode;
}

void Chip8::CPU::decodeOpcode(const uint16_t &opcode) {
  nibbles.opcode = opcode;

  nibbles.first = opcode & 0xF000;
  nibbles.sec = opcode & 0x0F00;
  nibbles.third = opcode & 0x00F0;
  nibbles.fourth = opcode & 0x000F;

  nibbles.lastTwo = (nibbles.third | nibbles.fourth);

  // printf("%0X %0X %0X %0X\n", nibbles.first, nibbles.sec, nibbles.third,
  // nibbles.fourth);
}

void Chip8::CPU::executeOpcode(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH>
        &pixels) {
  switch (nibbles.first) {

  case 0x0000:

    switch (nibbles.sec | nibbles.lastTwo) {
    case 0xE0:
      op00E0(pixels);
      break;

    case 0xEE:
      op00EE();
      break;

    default:
      printOpcodeMissing();
      break;
    }

    break;

  case 0x1000:
    op1NNN();
    break;

  case 0x2000:
    op2NNN();
    break;

  case 0x3000:
    op3XNN();
    break;

  case 0x4000:
    op4XNN();
    break;

  case 0x5000:
    op5XY0();
    break;

  case 0x6000:
    op6XNN();
    break;

  case 0x7000:
    op7XNN();
    break;

  case 0x8000:
    switch (nibbles.fourth) {
    case 0x0:
      op8XY0();
      break;

    case 0x1:
      op8XY1();
      break;

    case 0x2:
      op8XY2();
      break;

    case 0x3:
      op8XY3();
      break;

    case 0x4:
      op8XY4();
      break;

    case 0x5:
      op8XY5();
      break;

    case 0x6:
      op8XY6();
      break;

    case 0x7:
      op8XY7();
      break;

    case 0xE:
      op8XYE();
      break;

    default:
      printOpcodeMissing();
      break;
    }

    break;

  case 0x9000:
    op9XY0();
    break;

  case 0xA000:
    opANNN();
    break;

  case 0xB000:
    opBNNN();
    break;

  case 0xC000:
    opCXNN();
    break;

  case 0xD000:
    opDXYN(pixels);
    break;

  case 0xE000:

    switch (nibbles.lastTwo) {
    case 0x9E:
      opEX9E();
      break;

    case 0xA1:
      opEXA1();
      break;

    default:
      printOpcodeMissing();
      break;
    }

    break;

  case 0xF000:

    switch (nibbles.lastTwo) {

    case 0x07:
      opFX07();
      break;

    case 0x0A:
      opFX0A();
      break;

    case 0x15:
      opFX15();
      break;

    case 0x18:
      opFX18();
      break;

    case 0x1E:
      opFX1E();
      break;

    case 0x29:
      opFX29();
      break;

    case 0x33:
      opFX33();
      break;

    case 0x55:
      opFX55();
      break;

    case 0x65:
      opFX65();
      break;

    default:
      printOpcodeMissing();
      break;
    }

    break;

  default:
    printOpcodeMissing();
    break;
  }
}

void Chip8::CPU::printOpcodeMissing() {
  std::cout << "Error! Opcode " << std::hex << nibbles.opcode
            << " not implemented.\n";
}

// Opcode functions

// Clear screen op
void Chip8::CPU::op00E0(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH>
        &pixels) {
  drawFlag = true;

  pixels.fill(0);
}

// Return from subroutine
void Chip8::CPU::op00EE() {
  // Check if mStackptr underflows
  // mStackptr - 1 < 0
  // mStackptr < 1

  // Pop stack and jump to address
  mStackptr--;
  mPC = mStack.at(mStackptr);
}

// Jump instruction; PC jumps to NNN
void Chip8::CPU::op1NNN() {
  mPC = (nibbles.sec | nibbles.third | nibbles.fourth);
}

// Store current PC on stack, then jump to NNN
void Chip8::CPU::op2NNN() {
  mStack.at(mStackptr) = mPC;
  mStackptr++; // Should never be greater than 16!
  if (mStackptr > 16) {
    std::cout << "mStackPtr somehow greater than 16!\n";
  }

  mPC = (nibbles.sec | nibbles.third | nibbles.fourth);
}

// If VX == NN, skip one instruction
void Chip8::CPU::op3XNN() {
  if (V[nibbles.sec >> 8] == (nibbles.third | nibbles.fourth)) {
    mPC += 2;
  }
}

// If VX != NN, skip one instruction
void Chip8::CPU::op4XNN() {
  if (V[nibbles.sec >> 8] != (nibbles.third | nibbles.fourth)) {
    mPC += 2;
  }
}

// If VX == VY, skip one instruction
void Chip8::CPU::op5XY0() {
  if (V[nibbles.sec >> 8] == V[nibbles.third >> 4]) {
    mPC += 2;
  }
}

// VX = NN
void Chip8::CPU::op6XNN() {
  V[nibbles.sec >> 8] = (nibbles.third | nibbles.fourth);
}

// VX += NN
void Chip8::CPU::op7XNN() {
  V[nibbles.sec >> 8] += (nibbles.third | nibbles.fourth);
}

// VX = VY
void Chip8::CPU::op8XY0() { V[nibbles.sec >> 8] = V[nibbles.third >> 4]; }

// VX = VX | VY
void Chip8::CPU::op8XY1() {
  V[nibbles.sec >> 8] |= V[nibbles.third >> 4];
  V[0xF] = 0;
}

// VX = VX & VY
void Chip8::CPU::op8XY2() {
  V[nibbles.sec >> 8] &= V[nibbles.third >> 4];
  V[0xF] = 0;
}

// VX = VX ^ VY
void Chip8::CPU::op8XY3() {
  V[nibbles.sec >> 8] ^= V[nibbles.third >> 4];
  V[0xF] = 0;
}

// VX = VX + VY
void Chip8::CPU::op8XY4() {
  // If VX + VY > 0xFF (max value for unsigned 8bit), then set carry flag
  // Adjust equation to: VX > 0xFF - VY
  bool carryFlag = V[nibbles.sec >> 8] > 0xFF - V[nibbles.third >> 4];

  V[nibbles.sec >> 8] += V[nibbles.third >> 4];

  if (carryFlag) {
    V[0xF] = 1; // Set carry flag
  }

  else {
    V[0xF] = 0;
  }
}

// VX = VX - VY
void Chip8::CPU::op8XY5() {
  // VX - VY < 0 - underflow; need to borrow from VF
  // VX < VY
  bool carryFlag = V[nibbles.sec >> 8] < V[nibbles.third >> 4];

  V[nibbles.sec >> 8] -= V[nibbles.third >> 4];

  // VX < VY; borrow occurred
  if (carryFlag) {
    V[0xF] = 0;
  }

  // VX > VY; no borrow
  else {
    V[0xF] = 1;
  }
}

// Bit shift right
void Chip8::CPU::op8XY6() {
  if (quirkShift) {
    // Modern behavior; VX = VY
    V[nibbles.sec >> 8] = V[nibbles.third >> 4];

    // Requires a temp variable in the edge case of 8FF6
    // If we set VF to corresponding bit first, then we have just overwritten
    // VF!
    uint8_t carryBit =
        V[nibbles.sec >> 8] & 0b1; // Store last bit into temp variable
    V[nibbles.sec >> 8] >>= 1;     // Bitshift right in place

    V[0xF] = carryBit; // Set register to last bit
  }
}

// VX = VY - VX
void Chip8::CPU::op8XY7() {
  // VY - VX < 0 - underflow; need to borrow from VF
  // VY < VX
  bool carryFlag = V[nibbles.third >> 4] < V[nibbles.sec >> 8];

  V[nibbles.sec >> 8] = V[nibbles.third >> 4] - V[nibbles.sec >> 8];

  // VY < VX; borrow occurred
  if (carryFlag) {
    V[0xF] = 0;
  }

  // VY > VX; no borrow
  else {
    V[0xF] = 1;
  }
}

// Bit shift left
void Chip8::CPU::op8XYE() {
  if (quirkShift) {
    // Modern behavior; VX = VY
    V[nibbles.sec >> 8] = V[nibbles.third >> 4];

    // Requires a temp variable in the edge case of 8FFE
    // If we set VF to corresponding bit first, then we have just overwritten
    // VF!
    uint8_t carryBit =
        V[nibbles.sec >> 8] >> 7; // Store first bit into temp variable
    V[nibbles.sec >> 8] <<= 1;    // Bitshift left in place

    V[0xF] = carryBit; // Set register to first bit
  }
}

// If VX != VY, skip
void Chip8::CPU::op9XY0() {
  if (V[nibbles.sec >> 8] != V[nibbles.third >> 4]) {
    mPC += 2;
  }
}

// Set index register I to 0xNNN
void Chip8::CPU::opANNN() {
  I = (nibbles.sec | nibbles.third | nibbles.fourth);
}

// PC = NNN + V0
void Chip8::CPU::opBNNN() {
  mPC = (nibbles.sec | nibbles.third | nibbles.fourth) + V[0];
}

// Set VX to a random number, AND'd with NN
void Chip8::CPU::opCXNN() {
  V[nibbles.sec >> 8] =
      (rand() % (0xFF + 1)) & (nibbles.third | nibbles.fourth);
}

// Refer to:
// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font
// https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
void Chip8::CPU::opDXYN(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH>
        &pixels) {
  drawFlag = true;

  uint8_t x = V[nibbles.sec >> 8] % 64; // X, Y coordinates
  uint8_t y = V[nibbles.third >> 4] % 32;
  uint8_t N = nibbles.fourth; // This is our N pixels

  // Screen X coordinates range from 0 - 63
  // To make coordinates wrap, set X = X % 64. This gives remainder which is our
  // new coordinate

  V[0xF] = 0;
  for (int yLine = 0; yLine < N; yLine++) {
    if (y + yLine > Constants::BASE_HEIGHT - 1) {
      break;
    }

    uint8_t sprite = mMemory.at(I + yLine); // Grab sprite data

    for (int xLine = 0; xLine < 8; xLine++) {
      if (x + xLine > Constants::BASE_WIDTH - 1) {
        break;
      }

      // Grab each pixel bit from left to right.
      // Note that 0x80 is 0b1000 0000
      // Compare bit against current screen pixel
      uint8_t pixel = sprite & (0x80 >> xLine);

      if (pixel) {
        if (PixelFunctions::getPixel(pixels, x + xLine, y + yLine) == 1) {
          V[0xF] = 1;
        }

        // pixels.xorPixel(x + xLine, y + yLine, 1);
        // PIXEL_COLOR *pixelPtr =
        //     &PixelsFunction::getPixel(pixels, x + xLine, y + yLine);
        PixelFunctions::xorPixel(pixels, x + xLine, y + yLine);
      }

      // DO NOT INCREMENT X!!
      // x++;
    }

    // DO NOT INCREMENT Y!!
    // y++;
  }
}

void Chip8::CPU::opEX9E() // Check if key in VX is pressed or not. If so, skip
                          // an instruction
{
  if (mInternalKeys[V[nibbles.sec >> 8]] == true) {
    // std::cout << "Key detected\n";
    mPC += 2;
  }
}

void Chip8::CPU::opEXA1() // Check if key in VX is pressed or not. If not, skip
                          // an instruction
{
  if (mInternalKeys[V[nibbles.sec >> 8]] == false) {
    mPC += 2;
  }
}

void Chip8::CPU::opFX07() // VX = delayTimer
{
  V[nibbles.sec >> 8] = delayTimer;
}

// Wait on any key input. Loop until an input is received
void Chip8::CPU::opFX0A() {
  // std::cout << "Waiting on input; 0xFX0A...\n";
  bool anyKeyPressed = false;

  for (int i = 0; i < NUM_KEYS; i++) {
    if (mInternalKeys[i] == true) {
      anyKeyPressed = true;
      keyWasPressedLF = true;
      keyLastPressed = i;
    }
  }

  // Key was pressed last frame
  if (keyWasPressedLF) {
    if (anyKeyPressed) // Loop until key is released!
    {
      mPC -= 2;
    }

    else // Key is released on current frame
    {
      keyWasPressedLF = false;

      // Send hexadecimal value of RELEASED char to VX
      V[nibbles.sec >> 8] = keyLastPressed;
    }
  }

  else if (!anyKeyPressed) {
    mPC -= 2; // If no input is received, loop this instruction
  }
}

// delayTimer = VX
void Chip8::CPU::opFX15() { delayTimer = V[nibbles.sec >> 8]; }

// soundTimer = VX
void Chip8::CPU::opFX18() { soundTimer = V[nibbles.sec >> 8]; }

// I += VX
void Chip8::CPU::opFX1E() { I += V[nibbles.sec >> 8]; }

// Load font character hexadecimal from VX into I
void Chip8::CPU::opFX29() { I = (V[nibbles.sec >> 8] * 0x5) + FONT_LOCATION; }

// TODO: Document this function
void Chip8::CPU::opFX33() {
  // VX = 0dXYZ
  mMemory.at(I) = V[nibbles.sec >> 8] / 100;              // Grab X
  mMemory.at(I + 1) = ((V[nibbles.sec >> 8]) / 10) % 10;  // Grab Y
  mMemory.at(I + 2) = ((V[nibbles.sec >> 8]) % 100) % 10; // Grab Z
}

// TODO: Document this function
void Chip8::CPU::opFX55() {
  int targetRegister = nibbles.sec >> 8;

  for (int i = 0; i <= targetRegister; i++) {
    mMemory.at(I + i) = V[i];
  }

  if (quirkLoadStore) {
    I += targetRegister + 1; // Classic behavior; disable for modern
  }
}

// TODO: Document this function
void Chip8::CPU::opFX65() {
  int targetRegister = nibbles.sec >> 8;

  for (int i = 0; i <= targetRegister; i++) {
    V[i] = mMemory.at(I + i);
  }

  if (quirkLoadStore) {
    I += targetRegister + 1; // Classic behavior; disable for modern
  }
}

int PixelFunctions::calculatePixel(int xCoord, int yCoord) {
  // Coordinates should range from 0-63, 0-31
  if (xCoord >= 64 || yCoord >= 32) {
    return 255;
  }

  // Calculate appropriate number of pixels to travel
  int height = Constants::BASE_WIDTH * yCoord;

  return height + xCoord;
}

uint8_t PixelFunctions::getPixel(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels,
    int xCoord, int yCoord) {
  int pixel = calculatePixel(xCoord, yCoord);

  return pixels[pixel];
}

void PixelFunctions::xorPixel(
    std::array<uint8_t, Constants::BASE_HEIGHT * Constants::BASE_WIDTH> &pixels,
    int xCoord, int yCoord) {
  int pixelCoord = calculatePixel(xCoord, yCoord);

  pixels[pixelCoord] ^= 1;
}
