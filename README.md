# CHIP-8 Emulator

A CHIP-8 emulator written in C++20, with an SFML 3 window for display and keyboard input.

The emulator core has no dependency on SFML or the filesystem. It is a small class that takes ROM bytes, executes instructions, and exposes the display, so it can be driven by any front end or by tests.

<!-- Add a screenshot here, e.g. ![IBM logo](docs/screenshot.png) -->
[IBM logo](assets/IBM-logo.png)
## Status

Behaves as **original CHIP-8** (COSMAC VIP style).

- [x] All CHIP-8 instructions implemented
- [x] Passes every line of the `5-quirks` test ROM on the CHIP-8 platform
- [x] Runs the IBM logo test ROM and several demo ROMs
- [x] `3-corax+`, `4-flags` and `6-keypad` test ROMs: verified, all of them executes correctly
- [ ] Sound (the sound timer runs, but no tone is played)
- [ ] Configurable quirks / SUPER-CHIP and XO-CHIP support
- [ ] Automated unit tests

## Requirements

- A C++20 compiler (developed with GCC / MinGW-w64 on Windows)
- [CMake](https://cmake.org/) 3.22 or newer
- [SFML 3](https://www.sfml-dev.org/) (Graphics module)
- Optional: [Ninja](https://ninja-build.org/) for faster builds

## Build

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To choose a generator explicitly, add `-G Ninja` or `-G "MinGW Makefiles"`. If you change generators, delete the `build` folder first.

If CMake cannot find SFML, point it at your install:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH=<path to SFML install>
```

On Windows, the build copies the SFML DLLs next to the executable.

## Run

```
.\build\CHIP8.exe path\to\game.ch8
```

The window is 768 x 384 (the 64 x 32 display at 12x scale). Close the window to exit.

### Controls

The original 4x4 hex keypad is mapped to the left side of the keyboard:

```
CHIP-8 keypad        Keyboard
 1  2  3  C           2  3  4  Z
 4  5  6  D           Q  W  E  X
 7  8  9  E           R  A  S  C
 A  0  B  F           D  1  F  V
```

## ROMs

No ROMs are included. Some good ones to start with:

- [Timendus's CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite): logo tests, opcode tests, flags, quirks and keypad. Each ROM shows pass or fail marks on screen.
- Public-domain and homebrew games and demos, such as those collected in various CHIP-8 ROM archives. Check each ROM's license before redistributing it.

## Behavior (original CHIP-8 quirks)

These behaviors are currently fixed in the code:

| Quirk | Behavior |
|---|---|
| VF reset | `8xy1`, `8xy2`, `8xy3` set VF to 0 |
| Memory | `Fx55` / `Fx65` leave `I` advanced by `x + 1` |
| Display wait | `Dxyn` waits for the next 60 Hz tick |
| Clipping | Sprites are clipped at the screen edges (the start position wraps) |
| Shifting | `8xy6` / `8xyE` shift `Vy` and store the result in `Vx` |
| Jumping | `Bnnn` jumps to `nnn + V0` |

## Project layout

```
include/    Public headers (Machine, configuration constants, instruction decoding)
src/
  Machine.cpp   Emulator core: fetch, decode, execute
  main.cpp      SFML front end: window, input, ROM loading, main loop
CMakeLists.txt
```

CMake builds the core as a static library (`chip8_core`) and links it into the `CHIP8` executable.

### Core interface

```cpp
Chip8::Machine machine;

machine.Load(rom_bytes);        // LoadResult::Ok or TooLarge
machine.SetKey(key, down);      // key 0x0 - 0xF
machine.Step();                 // run one instruction, returns a StepResult
machine.TickTimers();           // call at 60 Hz
machine.Display();              // const view of the 64x32 pixel buffer
machine.SoundActive();          // true while the sound timer is running (planned to implement!)
```

`StepResult` reports `Ok`, `WaitingForKey` (not an error), `UnknownOpcode`, `StackOverflow` or `StackUnderflow`, so the front end decides how to handle problems.

### Main loop

The front end runs 10 instructions per frame at 60 frames per second (about 600 instructions per second) and ticks the timers once per frame. These values are constants at the top of `src/main.cpp`.

## Known limitations

- No sound yet.
- Frame timing uses SFML's frame-rate limit, so timers can drift slightly if a frame runs long.
- Instruction speed is fixed; there is no way to change it while running.
- Quirks cannot be switched, so games written for SUPER-CHIP or later variants may not run correctly.
- No pause, reset or drag-and-drop loading.

## Planned

- Beeper for the sound timer
- Fixed-rate timing (60 Hz timers independent of instruction rate)
- A `Quirks` configuration with presets for other platforms
- Unit tests for the core, run through CTest

## Resources

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Timendus's CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite)

## License

The MIT License (MIT)

Copyright (c) 2015 Chris Kibble

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

