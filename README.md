# Yet Another CHIP-8 Emulator

Another implementation of a CHIP-8 emulator, written in C with SDL2 for graphics and audio. So far I have confirmed that it builds.
I cannot speak to the performance yet.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
  - [Installing SDL2](#installing-sdl2)
- [Building the Emulator](#building-the-emulator)
- [Running the Emulator](#running-the-emulator)
- [Controls](#controls)
- [Logging](#logging)
- [Resources](#resources)
- [License](#license)

## Features

- **CPU Emulation**: All 35 instructions - see ISA.md for details. Implemented in chip8.c
- **Graphics**: Renders display with SDL2 - see src/graphics.c 
- **Input Handling**: 16 key input 
- **Audio**: Buzzer is emulated by generating a square wave - again with SDL2 - see src/audio.c .
- **Logging**: For debugging purposes. See src/logger.c

## Prerequisites

To build and run the emulator, you need:

- **C Compiler**: GCC or any standard-compliant C compiler.
- **Make**: To use the provided Makefile.
- **SDL2**: Simple DirectMedia Layer library for cross-platform development.

### Installing SDL2

#### Linux

**Ubuntu/Debian:**

```bash
sudo apt-get update
sudo apt-get install libsdl2-dev
```

## Building the Emulator

1. **Clone or Download the Repository:**

    ```bash
    git clone https://github.com/columbladee/yac8emu.git
    cd yac8emu
    ```

2. **Ensure the Logs Directory Exists:**

    The emulator logs information to a `logs` directory. Create it if it doesn't exist:

    ```bash
    mkdir -p logs
    ```

3. **Build Using Make:**

    ```bash
    make
    ```

    This will compile the source files and produce an executable named `chip8_emulator`.

4. **Clean Build Artifacts (Optional):**

    To clean up object files and the executable:

    ```bash
    make clean
    ```

## Running the Emulator

```bash
./chip8_emulator path/to/your/rom.ch8
```

- Replace `path/to/your/rom.ch8` with the actual path to the CHIP-8 ROM file you want to run.
- Ensure that the ROM file exists and is accessible.

**Example:**

```bash
./chip8_emulator roms/PONG.ch8
```

## Controls

The CHIP-8 emulator maps the original hexadecimal keypad to your keyboard as follows:

```
+-----+-----+-----+-----+
|  1  |  2  |  3  |  4  |       
| 0x1 | 0x2 | 0x3 | 0xC |       
+-----+-----+-----+-----+      
|  Q  |  W  |  E  |  R  |       
| 0x4 | 0x5 | 0x6 | 0xD |   
+-----+-----+-----+-----+  
|  A  |  S  |  D  |  F  |      
| 0x7 | 0x8 | 0x9 | 0xE |       
+-----+-----+-----+-----+      
|  Z  |  X  |  C  |  V  |    
| 0xA | 0x0 | 0xB | 0xF |       
+-----+-----+-----+-----+        
```

- **Exit Emulator**: Press `ESCAPE` or close the window.

## Logging

- The emulator generates logs in the `logs/chip8_emulator.log` file.
- Logs include information, warnings, errors, and debug messages.
- Use logs to troubleshoot and understand emulator behavior.

## Resources

- **CHIP-8 Instruction Set Reference**: [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- **CHIP-8 Design Specification**: [Columbia University Embededded Systems 4840 CHIP-8 Design Specification] (https://www1.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf)
- **CHIP-8 Hardware Emulator**:[CSEE 4840 Embedded Systems: CHIP-8 Hardware Emulator] (https://www.cs.columbia.edu/~sedwards/classes/2022/4840-spring/designs/CHIP-8.pdf)
- **CHIP-8 Wiki**:[mattmikolay's Github Wiki page] (https://github.com/mattmikolay/chip-8/wiki)
- **ROMs**: Search online lol
- **SDL2 Documentation**: [SDL2 Wiki](https://wiki.libsdl.org/SDL2)
- **Misc. Audio information**: [BasicSynth] (http://basicsynth.com/index.php?page=basic) 
- **Misc: Textbook/Notes I found**: [Under the Covers : The Secret Life of Software - Boston University’s College of Arts and Sciences Textbook] (https://jappavoo.github.io/UndertheCovers/textbook/intro_tb.html)
- **Misc: Textbook/Notes I found**: [Under the Covers : The Secret Life of Software - Boston University’s College of Arts and Sciences Lecture Notes] (https://jappavoo.github.io/UndertheCovers/textbook/intro_tb.html)



## License

This project is licensed under the GPL-3 License.


