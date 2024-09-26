# CHIP-8 Instruction Set / Opcode Documentation

## Initial Note:

1. **I am aware a lot of this is redundant.** Description will sometimes be identical (if not very similar) to the implementation. I prefer a consistent formatting style and saying "See description" over leaving the reader to assume the description and implementation are equivalent.
When I say "Self explanatory", I am using this in the general sense because I am writing this doc out before I implement it in functions I have yet to prototype and might revisit later. Probably not. But the implementation isn't exactly "self explanatory" in that case". 

2. **The implementation is barely implementation as it should be**. I know. Again, I don't know where I'm implementing all of these yet. Deal with it.

3. **By the time I finished this markdown doc, I realized I could have formatted it differently to be more concise.** It gets the job done though. Also deal with it.

4. I have done my best to match the hardware docs to the files I actually use here.

## Instructions (Opcodes):

### Credits/Source: 

**Note**: I am merely summarizing documentation found here:

1. [CHIP-8 Wikipedia Article](https://en.wikipedia.org/wiki/CHIP-8)
2. [Learn CHIP-8 - Cowgod's Guide](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

### `00E0` - Clear the Display
- **Description**: Clears display
- **Effect**: Sets all pixel values to 0
- **Implementation**: Calls the `clearDisplay()` , sets draw flag to true so screen can be refreshed

### `00EE` - Return from Subroutine
- **Description**: Returns from a subroutine by popping the program counter (PC) from the stack.
- **Effect**: Decreases the stack pointer (SP) and sets the PC to the value stored on the stack.
- **Implementation**: Decrements `SP` and restores the `PC` from the stack.

### `1NNN` - Jump to Address NNN
- **Description**: Sets the program counter (PC) to address `NNN`.
- **Effect**: Makes the CPU jump to a specific address.
- **Implementation**: `chip8->PC = opcode & 0x0FFF`.

### `2NNN` - Call Subroutine at NNN
- **Description**: Calls a subroutine at address `NNN`.
- **Effect**: Pushes the current PC to the stack and then sets the PC to `NNN`.
- **Implementation**: The current PC is saved to the stack and the subroutine's address is set in `PC`.

### `3XNN` - Skip Next Instruction if VX == NN
- **Description**: Skips the next instruction if the value in register `VX` equals `NN`.
- **Effect**: If `VX == NN`, the program counter is increased by 2, skipping the next instruction.
- **Implementation**: Checks `VX`, and if it equals `NN`, increments the PC by 4 (2 instructions).

### `4XNN` - Skip Next Instruction if VX != NN
- **Description**: Skips the next instruction if the value in register `VX` does not equal `NN`.
- **Effect**: If `VX != NN`, the program counter is increased by 2, skipping the next instruction.
- **Implementation**: Checks `VX`, and if it DNE `NN`, increments the PC by 4 (2 instructions).

### `5XY0` - Skip Next Instruction if VX == VY
- **Description**: Skips the next instruction if `VX` equals `VY`.
- **Effect**: If `VX == VY`, the program counter is increased by 2, skipping the next instruction.
- **Implementation**: Compares registers `VX` and `VY`, and if equal, increases the PC by 4.

### `6XNN` - Set VX to NN
- **Description**: Sets register `VX` to the value `NN`.
- **Effect**: Stores the immediate value `NN` in `VX`.
- **Implementation**: `chip8->V[X] = NN`.

### `7XNN` - Add NN to VX
- **Description**: Adds the value `NN` to `VX`, storing the result in `VX`.
- **Effect**: `VX = VX + NN`.
- **Implementation**: Self explanatory - see description...

### `8XY0` - Set VX to VY
- **Description**: Sets `VX` equal to `VY`.
- **Effect**: Copies the value in `VY` to `VX`.
- **Implementation**: `chip8->V[X] = chip8->V[Y]`.

### `8XY1` - Set VX to VX OR VY
- **Description**: Performs a bitwise OR on `VX` and `VY` and stores the result in `VX`.
- **Effect**: `VX = VX | VY`.
- **Implementation**: Self explanatory -  see description ...

### `8XY2` - Set VX to VX AND VY
- **Description**: Performs a bitwise AND on `VX` and `VY` and stores the result in `VX`.
- **Effect**: `VX = VX & VY`.
- **Implementation**: Self explanatory -  see description ...

### `8XY3` - Set VX to VX XOR VY
- **Description**: Performs a bitwise XOR on `VX` and `VY` and stores the result in `VX`.
- **Effect**: `VX = VX ^ VY`.
- **Implementation**: Self explanatory -  see description ...

### `8XY4` - Add VY to VX, Set VF on Carry
- **Description**: Adds `VY` to `VX`, and sets the carry flag `VF` if the result overflows.
- **Effect**: `VX = VX + VY`, `VF` is set if there's a carry.
- **Implementation**: Adds `VY` to `VX` and sets the carry flag `VF` accordingly.

### `8XY5` - Subtract VY from VX, Set VF if No Borrow
- **Description**: Subtracts `VY` from `VX` and sets `VF` to 1 if no borrow occurs.
- **Effect**: `VX = VX - VY`, `VF` is set if there's no borrow.
- **Implementation**: Self explanatory -  see description ...

### `8XY6` - Shift VX Right by 1, Store Least Significant Bit in VF
- **Description**: Shifts `VX` right by 1, storing the least significant bit in `VF`.
- **Effect**: `VX = VX >> 1`, `VF` = least significant bit of `VX`.
- **Implementation**: Right-shifts `VX` and stores the least significant bit in `VF`.

### `8XY7` - Set VX to VY - VX, Set VF if No Borrow
- **Description**: Sets `VX` to `VY - VX`, and sets the borrow flag `VF` if there's no borrow.
- **Effect**: `VX = VY - VX`, `VF` is set if there's no borrow.
- **Implementation**: Subtracts `VX` from `VY` and stores the result in `VX`, adjusting `VF`.

### `8XYE` - Shift VX Left by 1, Store Most Significant Bit in VF
- **Description**: Shifts `VX` left by 1, storing the most significant bit in `VF`.
- **Effect**: `VX = VX << 1`, `VF` = most significant bit of `VX`.
- **Implementation**: Self explanatory -  see description ...

### `ANNN` - Set I to Address NNN
- **Description**: Sets register `I` to the address `NNN`.
- **Effect**: `I = NNN`.
- **Implementation**: `chip8->I = NNN`.

### `CXNN` - Set VX to Random Byte AND NN
- **Description**: Generates a random number, ANDs it with `NN`, and stores the result in `VX`.
- **Effect**: `VX = (random byte) & NN`.
- **Implementation**: Self explanatory -  see description ...

### `DXYN` - Display N-byte Sprite Starting at I at (VX, VY), Set VF on Collision
- **Description**: Draws an N-byte sprite starting at memory address `I` at coordinates `(VX, VY)` on the display. `VF` is set to 1 if any pixels are flipped from set to unset (collision detection).
- **Effect**: Draws the sprite on the display and sets the collision flag `VF`.
- **Implementation**: `drawSprite()` function is used to draw the sprite and detect collisions.

### `EX9E` - Skip Next Instruction if Key in VX is Pressed
- **Description**: Skips the next instruction if the key corresponding to the value in `VX` is pressed.
- **Effect**: If the key in `VX` is pressed, the PC skips the next instruction.
- **Implementation**: Checks the keypad array and skips if the key is pressed.

### `EXA1` - Skip Next Instruction if Key in VX is Not Pressed
- **Description**: Skips the next instruction if the key corresponding to the value in `VX` is not pressed. DO NOT CONFUSE THIS WITH `EX9E`
- **Effect**: If the key in `VX` is not pressed, the PC skips the next instruction.
- **Implementation**: Checks the keypad array and skips if the key is not pressed. AGAIN DO NOT CONFUSE THIS WITH `EX9E` IT LOOKS VERY SIMILAR IN THIS DOCUMENTATION.

### `FX07` - Set VX to Value of Delay Timer
- **Description**: Sets `VX` to the current value of the delay timer.
- **Effect**: `VX = delayTimer`.
- **Implementation**: Checks the keypad array and skips if the button isn't pressed.

### `FX0A` - Wait for Key Press, Store Value in VX
- **Description**: Waits for key press and stores value in VX
- **Effect**: Pauses execution until a key is press then stores value.
- **Implementation**: Loops until a key is pressed ... 

### `FX15` - Set Delay Timer to VX
- **Description**: Sets the delay timer to value in VX.
- **Effect**: `delayTimer = VX`
- **Implementation**: Copies value of `VX` to delay timer

### `FX18` - Set Sound Timer to VX
- **Description**: Sets the sound timer to value in VX. 
- **Effect**: `soundTimer = VX`
- **Implementation**: Copies value of `VX` to sound timer... 

### `FX1E` - Add VX to I
- **Description**: What do you think? Adds value in VX to register I.
- **Effect**: `I += VX`
- **Implementation**:  Self explanatory -  see description ...

### `FX29` - Set I to Location of Sprite for Char in VX
- **Description**: Sets I to memory location of the sprite (representing the character in `VX`). NOTE: 0-F are stored at `0x50`
- **Effect**: `I = location of sprite for char VX`
- **Implementation**:  Self explanatory -  see description ...

### `FX33` - Store Binary-Coded Decimal (BCD) in I, I+1, I+2
- **Description**: Stores BCD representation of `VX` at `I`, `I+1`, and `I+2`
- **Effect**: Stores the hundreds, tens, and ones digits of `VX`
- **Implementation**: Breaks `VX` into digits and stores them in three (consecutive) memory locations... this isn't implementation, more like an extended description.

### `FX55` - Store V0 to VX in Memory starting at I
- **Description**: Stores registers from `V0` to `VX` in memory starting at address `I`
- **Effect**: Stores values of `V0` to `VX` in  memory
- **Implementation**:  Self explanatory -  see description ...

### `FX65` - Read V0 to VX from Memory starting at I
- **Description**: Opposite of `FX55` - reads values from memory starting at `I` into registers from `V0` to `VX`
- **Effect**: Loads values from memory into registers `V0` through `VX`
- **Implementation**: Self explanatory -  see description ...

