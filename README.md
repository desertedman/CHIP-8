# CHIP-8 Emulator

My own personal CHIP-8 Emulator built in C++ for Linux systems. Built and tested
on Fedora 42!

[Video demo](https://youtu.be/xRnYh-q6Ma4?si=vIn9rEbaX7bk5Soe)

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

### 1. Install Dependencies

#### Linux (Fedora 42)

- CMake
- G++
- SDL2 Development Libraries

These may be easily installed by running the following in the terminal:

```bash
sudo dnf install SDL2-devel cmake gcc-c++
```

Other distros should be able to install the corresponding packages with
relative ease.

#### Windows

- [CMake](https://cmake.org/download/)
- Visual Studio (MSVC) or [MinGW](https://code.visualstudio.com/docs/cpp/config-mingw)

### 2. Clone Repository and Generate Files

```bash
git clone --recurse-submodules https://github.com/desertedman/CHIP-8.git
cd CHIP-8
```

#### Linux / MinGW

```bash
# Linux
cmake -B build -S ./
# Alternatively on Windows,
# cmake -G "MinGW Makefiles" -B build -S ./

cd build
cmake --build ./
```

For MinGW and Linux users, a compiled executable should already be spit out in
the build directory.

### (Windows - Visual Studio) 3. Compile

Navigate to the generated build folder, open the .sln file, and configure your
project to use "CHIP8" as the startup project. You can do this by right
clicking the solution in the Solution Explorer -> Properties -> Set "Single
startup project" to "CHIP8". Compile the project, and you're good to go!

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

- Space Invaders does not function correctly. Space Invaders relies on SuperChip
quirks which I have chosen not to implement.

## TODO

- Write some information on 0xDXYN opcode. I found that all sources were fairly confusing!
- Windows cross-compatibility? (In progress)
- Save states?

## Resources

- The fine folks over at the [EmuDev discord](https://discord.gg/dkmJAes)
- <https://tobiasvl.github.io/blog/write-a-chip-8-emulator/>
- <https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/>
- <http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#Dxyn>
- <https://github.com/Timendus/chip8-test-suite>
- <https://github.com/JamesGriffin/CHIP-8-Emulator>
