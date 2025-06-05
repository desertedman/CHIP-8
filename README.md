# CHIP-8 Emulator

My own personal CHIP-8 Emulator built in C++ for Linux systems. Built and tested
on Fedora 42!



## Features

- Simple GUI via imgui integration
- Pause/unpause state
- Reset state
- Dynamic ROM loading
- Speed configuration

## Dependencies

Requires:

- SDL2
- CMake
- [imgui](https://github.com/ocornut/imgui) (included)
- [ImGui File Dialog](https://github.com/aiekick/ImGuiFileDialog) (included as submodule)

## Instructions

### Linux

#### Fedora 42

```bash
sudo dnf install SDL2-devel cmake gcc-c++
```

```bash
git clone --recurse-submodules https://github.com/desertedman/CHIP-8.git
cd CHIP-8
cmake -B build -S ./
cd build
cmake --build ./
```

### Windows
None yet

## Controls

```none
Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+
```

(Taken from [multigesture.net](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/))

### Hotkeys

- B to toggle GUI on and off
- Space to pause/unpause
- Enter to reset emulator
- ESC to quit

## Known Issues

- Resizing window larger than its original size crops the image
- Space Invaders does not function correctly. Space Invaders relies on SuperChip quirks which I have chosen not to implement.

## TODO

- Write some information on 0xDXYN opcode. I found that all sources were fairly confusing!
- Windows cross-compatibility?
- Save states?

## Resources

- The fine folks over at the [EmuDev discord](https://discord.gg/dkmJAes)
- <https://tobiasvl.github.io/blog/write-a-chip-8-emulator/>
- <https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/>
- <http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#Dxyn>
- <https://github.com/Timendus/chip8-test-suite>
- <https://github.com/JamesGriffin/CHIP-8-Emulator>
