#include "Chip8.h"
#include "Display.h"

#include <exception>
#include <iostream>
#include <memory>

int main() {
  // Roms
  std::string SpaceInvaders = "../roms/Space Invaders [David Winter].ch8";
  std::string Breakout = "../roms/breakout.rom";
  std::string Pong1 = "../roms/Pong (1 player).ch8";
  std::string Pong2 = "../roms/pong2.rom";
  std::string IBM = "../roms/IBM Logo.ch8";
  std::string Opcode = "../roms/3-corax+.ch8";
  std::string OpcodeAlt = "../roms/3-corax+_alt.ch8";
  std::string Flags = "../roms/4-flags.ch8";
  std::string Quirks = "../roms/5-quirks.ch8";
  std::string Input = "../roms/6-keypad.ch8";
  std::string Dummy = "./asdfdg";
  std::string LargeFile = "../roms/largeFile.txt";

  try {
    std::shared_ptr<Chip8> chip8 = std::make_shared<Chip8>();
    Display display(chip8);
    chip8->mDisplay = &display;

    std::cout << "Attempting to load file..." << std::endl;

    chip8->runEngine();
  }

  catch (const std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
  }

  return 0;
}
