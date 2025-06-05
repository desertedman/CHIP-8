# CHIP-8 Emulator

CHIP-8 Emulator built in C++ for Linux systems.

## Dependencies

Requires:

- SDL2

- CMake

- [imgui](https://github.com/ocornut/imgui) (included)

- [ImGui File Dialog](https://github.com/aiekick/ImGuiFileDialog) (included as submodule)

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

(Taken from multigesture.net)

## TODO

- Write some information on 0xDXYN opcode. I found that all sources were fairly confusing!

- Windows cross-compatibility?

- DONE: UI Integration, something like Dear ImGui

### Resources

- The fine folks over at the [EmuDev discord](https://discord.gg/dkmJAes)
- <https://tobiasvl.github.io/blog/write-a-chip-8-emulator/>
- <https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/>
- <http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#Dxyn>
- <https://github.com/Timendus/chip8-test-suite>
- <https://github.com/JamesGriffin/CHIP-8-Emulator>
