#include "chip8.h"
#include "sdl_wrapper.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>




// CHIP-8 fontset (contains hexadecimal digits 0-F, stored at memory locations 0x050 to 0x09F)
// 4KB memory, 16 gp registers, 16 levels of stack, 16-key hexadecimal keypad, 64x32 pixel display scaled up by 10 (640x320)
#define CHIP8_MEMORY_SIZE 4096 
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_KEYPAD_SIZE 16
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_DISPLAY_SIZE (CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT)
#define CHIP8_START_ADDRESS 0x200
#define CHIP8_FONTSET_START_ADDRESS 0x50
#define CHIP8_FONTSET_SIZE 80


static const uint8_t chip8_fontset[CHIP8_FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



/* Some initial Notes to help understand:

    1) The CHIP-8 has 35 opcodes, which are all two bytes long and stored big-endian.
    2) 16 Register count V[0...F] (e.g. V0, V1, ...) with a flag register VF (collision flag)
    3) Index Register I , Program Counter PC, and Stack Pointer SP
    3) Recall the chip8_t struct we made in chip8.h , which includes:
    uint8_t V[CHIP8_REGISTER_COUNT]; 
    uint16_t I; // Index register
    uint16_t PC; // Program counter
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t SP;
    uint8_t delay_timer;        
    uint8_t sound_timer;    
    uint8_t display[CHIP8_DISPLAY_SIZE]; 
    bool keypad[CHIP8_KEYPAD_SIZE];
    uint16_t opcode;
    bool drawFlag; 

    5. Important: chip8 is a pointer to a chip8_t struct, so we can access the fields of the struct using the -> operator.
     For example, to access the PC field of the chip8_t struct, we would use chip8->PC.
    6. To access or change a register's value in the V array, we can use the V array with the register number as the index.
    For example, to access register V5, we would use chip8->V[5]. chip8->V[X] means register V[X] in the chip8_t struct,
    where X is a hexadecimal digit (0-F).
    7. A fontset is a collection of sprites (characters) that are stored in memory. The CHIP-8 fontset contains hexadecimal digits 0-F, 
    each represented by a 4x5 pixel sprite. 

    (opcode & 0xF000) >> 12: Extracts the first 4 bits of the opcode (opcode & 0xF000) and shifts them to the right by 12 bits.
    Hopes the helps clear up some of the initial confusion later on in decodeAndExecute() ;)


    Some useful notes on Nibbles:

    Example:

For opcode 0x8123 (which is an 8XYN operation):

    First nibble (0x8000): Tells us this is a register-to-register operation (8XYN).
    Second nibble (0x0100): Extracts X = 1 → operates on VX.
    Third nibble (0x0020): Extracts Y = 2 → operates on VY.
    Fourth nibble (0x0003): Specifies the operation (XOR in this case).
*/


void initializeCPU(chip8_t *chip8) {
    chip8->PC = CHIP8_START_ADDRESS; // Program counter starts at 0x200
    chip8->I = 0; // Reset index register
    chip8->SP = 0; // Reset stack pointer

    //clear display and mem
    clearDisplay(chip8);
    memset(chip8->V, 0, sizeof(chip8->V));
    memset(chip8->stack, 0, sizeof(chip8->stack));
    memset(chip8->memory, 0, sizeof(chip8->memory));
    memset(chip8->keypad, 0, sizeof(chip8->keypad));

    // Load fontset
    for (int i = 0; i < CHIP8_FONTSET_SIZE; i++) {
        chip8->memory[CHIP8_FONTSET_START_ADDRESS + i] = chip8_fontset[i];
    }

    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    chip8->drawFlag = false;

    srand((unsigned int)time(NULL));

    logInfo("Initialized CPU");
}

void loadROM(chip8_t *chip8, const char *romPath) {
    FILE * rom = fopen(romPath, "rb");
    if (!rom) {
        logError("Failed to open ROM file: %s", romPath);
        exit(1);
    }

    fseek(rom, 0, SEEK_END);
    long romSize = ftell(rom);
    rewind(rom);

    if (romSize > (CHIP8_MEMORY_SIZE - CHIP8_START_ADDRESS)) {
        logError("ROM file too large: %ld bytes", romSize);
        fclose(rom);
        exit(1);
    }

    size_t bytesRead = fread(&chip8->memory[CHIP8_START_ADDRESS], sizeof(uint8_t), romSize, rom);
    if (bytesRead != romSize) {
        logWarning("Could not read the entire ROM file");
    }
    fclose(rom);
    
    logInfo("Loaded ROM: %s (%ld bytes)", romPath, romSize);
}

uint16_t fetchOpcode(chip8_t *chip8) {
    uint16_t opcode = (chip8->memory[chip8->PC] << 8) | chip8->memory[chip8->PC + 1];
    logDebug("Fetched opcode: 0x%04X at address: 0x%04X", opcode, chip8->PC);
    return opcode;
}

void executeCycle(chip8_t *chip8) {
    chip8->opcode = fetchOpcode(chip8);
    decodeAndExecute(chip8, chip8->opcode);
    updateTimers(chip8);

}

void decodeAndExecute(chip8_t *chip8, uint16_t opcode) {
    logDebug("Decoding opcode: 0x%04X", opcode);

    switch(opcode & 0xF000) {
        case 0x000: {
            switch (opcode & 0x00FF) {
                case 0x00E0: // 00E0: Clear the display
                    clearDisplay(chip8);
                    chip8->drawFlag = true; // Set flag to redraw screen
                    chip8->PC += 2; //  Move to next instruction
                    logInfo("Opcode 00E0: Clear the display");
                    break;

                case 0x00EE: // 00EE: Return from a subroutine
                    if (chip8->SP == 0) {
                        logError("Stack underflow on 00EE opcode");
                        exit(1);
                    }
                    chip8->SP--; // Decrement stack pointer
                    chip8->PC = chip8->stack[chip8->SP]; // Move to address at top of stack
                    chip8->PC += 2; // Move to next instruction
                    logInfo("Opcode 00EE: Return from a subroutine to 0x%04X", chip8->PC);
                    break;

                default:
                    logError("Unknown opcode: 0x%04X", opcode);
                    exit(1);
            }
            break;
        }
        case 0x1000:  // 1NNN: Jump to address NNN
            chip8->PC = opcode & 0x0FFF; // Set PC to address NNN
            logInfo("Opcode 1NNN: Jump to address 0x%04X", chip8->PC);
            break;

        case 0x2000: // 2NNN: Call subroutine at NNN
            if (chip8->SP == CHIP8_STACK_SIZE) {
                logError("Stack overflow on 2NNN opcode");
                exit(1);
            }
            chip8->stack[chip8->SP] = chip8->PC; // Store current address on stack
            chip8->SP++; // Increment stack pointer
            chip8->PC = opcode & 0x0FFF; // Jump to subroutine at NNN
            logInfo("Opcode 2NNN: Call subroutine at 0x%04X", chip8->PC);
            break;

        case 0x3000:  // 3XNN: Skip next instruction if VX equals NN
            if (chip8->V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                chip8->PC += 4; // Skip next instruction
                logInfo("Opcode 3XNN: Skip next instruction, V[%X] != 0x%02X", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
            }
        
        case 0x4000: // 4XNN - Skip next instruction if VX != NN
            if (chip8->V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                chip8->PC += 4; // Skip next instruction
                logInfo("Opcode 4XNN: Skip next instruction, V[%X] != 0x%02X", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
            } else {
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 4XNN: Do not skip next instruction, V[%X] == 0x%02X", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
            }
            break;

        case 0x5000: // 5XY0 - Skip next instruction if VX equals VY
            if (chip8->V[(opcode & 0x0F00) >> 8] == chip8->V[(opcode & 0x00F0) >> 4]) {
                chip8->PC += 4; // Skip next instruction
                logInfo("Opcode 5XY0: Skip next instruction, V[%X] == V[%X]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            } else {
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 5XY0: Do not skip next instruction, V[%X] != V[%X]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            }
            break;
        
        case 0x6000: // 6XNN - Set VX to NN
            chip8->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF; // Set VX to NN
            chip8->PC += 2; // Move to next instruction
            logInfo("Opcode 6XNN: Set V[%X] to 0x%02X", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
            break;
        
        case 0x7000: // 7XNN - Add NN to VX
            chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF; // Add NN to VX
            chip8->PC += 2; // Move to next instruction
            logInfo("Opcode 7XNN: Add 0x%02X to V[%X]", opcode & 0x00FF, (opcode & 0x0F00) >> 8);
            break;
        
        case 0x800: { // Handle math and logical operators between registers
        uint8_t X = (opcode & 0x0F00) >> 8;
        uint8_t Y = (opcode & 0x00F0) >> 4; 
        switch (opcode & 0x000F) { 
            case 0x0000: // 8XY0 - Set VX to the value of VY
                chip8->V[X] = chip8->V[Y];
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY0: Set V[%X] to V[%X]", X, Y);
                break;
            
            case 0x0001: // 8XY1 - Set VX to VX OR VY
                chip8->V[X] |= chip8->V[Y];
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY1: Set V[%X] to V[%X] OR V[%X]", X, X, Y);
                break;
            
            case 0x0002: // 8XY2 - Set VX to VX AND VY
                chip8->V[X] &= chip8->V[Y];
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY2: Set V[%X] to V[%X] AND V[%X]", X, X, Y);
                break;
            
            case 0x0003: // 8XY3 - Set VX to bitwise VX XOR VY
                chip8->V[X] ^= chip8->V[Y]; // VX ^= VY
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY3: Set V[%X] to V[%X] XOR V[%X]", X, X, Y);
                break;
            
            case 0x0004: { // 8XY4 - Add VY to VX, set VF on carry
                uint16_t sum = chip8->V[X] + chip8->V[Y];
                chip8->V[0xF] = (sum > 0xFF) ? 1 : 0; // Set VF to 1 if there is a carry
                chip8->V[X] = sum & 0xFF; // Store the lower 8 bits of the sum in VX
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY4: Add V[%X] to V[%X], set VF to %d", X, Y, chip8->V[0xF]);
                break;
            } 

            case 0x0005: { // 8XY5 : Subtract VY from VX, set VF if no borrow
                chip8->V[0xF] = (chip8->V[X] >= chip8->V[Y]) ? 1 : 0; // Set VF to 1 if there is no borrow
                chip8->V[X] -= chip8->V[Y]; // Subtract VY from VX
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY5: Subtract V[%X] from V[%X], result: 0x%02X, borrow: %d", X, Y, chip8->V[X], !chip8->V[0xF]);
                break;
            }
            
            case 0x0006: // 8XY6 - Store least significant bit of VX in VF, then shift VX to the right by 1
                chip8->V[0xF] = chip8->V[Y] >= chip8->V[X] ? 1 : 0; // Set VF to the least significant bit of VX
                chip8->V[X] = chip8->V[Y] - chip8->V[X]; //Subtract VX from VY
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY6: Store LSB of V[%X] in VF, then shift V[%X] to the right by 1", X, X);
                break;
            
            case 0x0007: { // 8XY7: Set VX to VY - VX, set VF if no borrow
                chip8->V[0xF] = chip8->V[Y] >= chip8->V[X] ? 1 : 0; // Set VF to 1 if there is no borrow
                chip8->V[X] = chip8->V[Y] - chip8->V[X]; // Subtract VX from VY
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XY7: Set V[%X] to V[%X] - V[%X], result: 0x%02X, borrow: %d", X, Y, X, chip8->V[X], !chip8->V[0xF]);
                break;
            }

            case 0x000E: { // 8XYE: Store most significant bit of VX in VF, then shift VX to the left by 1
                chip8->V[0xF] = (chip8->V[X] & 0x80) >> 7; // Set most significant bit of VX in VF
                chip8->V[X] <<= 1; // Shift VX to the left by 1
                chip8->PC += 2; // Move to next instruction
                logInfo("Opcode 8XYE: Shift V[%X] to the left by 1, result: %0x%02X , store MSB: %d in VF", X, chip8->V[X], chip8->V[0xF]);
                break;
            
            default:
                logError("Unknown opcode: 0x%04X", opcode);
                exit(1);
            }
            break;
        }

        case 0x9000: // 9XY0 : Skip next instruction if VX != VY
            if (chip8->V[(opcode & 0x0F00) >> 8] != chip8->V[(opcode & 0x00F0) >> 4]) {
                chip8->PC += 4; // Skip next instruction
                logInfo("Opcode 9XY0: Skip next instruction, V[%X] != V[%X]", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            } else {
                chip8->PC += 2; // Move to next instruction
            }
            break;
        
        case 0xA000: // ANNN: Set I to address NNN
            chip8->I = opcode & 0x0FFF; // Set I to address NNN
            chip8->PC += 2; // Move to next instruction
            logInfo("Opcode ANNN: Set I to 0x%04X", chip8->I);
            break;
        
        case 0xC000: // CXNN: Set VX to random byte AND NN
            chip8->V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF); // Set VX to random byte AND NN
            chip8->PC += 2; // Move to next instruction
            logInfo("Opcode CXNN: Set V[%X]  random byte &  0x%02X, result: 0x%02X", (opcode & 0x0F00) >> 8, opcode & 0x00FF, chip8->V[(opcode & 0x0F00) >> 8]);
            break;

        case 0xD000: { // DXYN: Display N-byute string starting at memory adress I at (VX, VY), set VF = collision
            uint8_t x = chip8->V[(opcode & 0x0F00) >> 8]; // X coordinate from VX
            uint8_t y = chip8->V[(opcode & 0x00F0) >> 4]; // Y coordinate from VY
            uint8_t height = opcode & 0x000F; // Height (N bytes) from the last sprite
            const uint8_t *sprite = &chip8->memory[chip8->I]; // Sprite data starting at memory address I

            bool collision = drawSprite(chip8, x, y, sprite, height); // Draw sprite on display
            chip8->V[0xF] = collision ? 1: 0; // Set VF to 1 if collision detected
            chip8->drawFlag = true; // Set flag to redraw screen
            chip8->PC += 2; // Move to next instruction
            logInfo("Opcode DXYN: Draw sprite at (%d, %d), height: %d, collision (bool): %d", X, Y, height, collision);
            break;           
        }

        case 0xE000:
            // Opcodes below handle keypress events
            switch (opcode & 0x00FF) {
                case 0x009E: // EX9E : Skip next instruction if key stored in VX is pressed
                    if (chip8->keypad[chip8->V[(opcode & 0x0F00) >> 8]]) {
                        chip8->PC += 4; // Skip next instruction
                        logInfo("Opcode EX9E: Skip next instruction, key in V[%X]=0x%X is pressed", (opcode & 0x0F00) >> 8, chip8->V[(opcode & 0xF00) >> 8]);
                    } else {
                        chip8->PC += 2; // Move to next instruction
                    }
                    break;

                    default: 
                        logError("Unknown opcode: 0x%X", opcode);
                        exit(1);
            }
            break;

            case 0xF000:
            // Handle timers, memory, input
            switch (opcode & 0x00FF) {
                case 0x007: // FX07 : Set VX to value of the delay timer
                    chip8->V[(opcode & 0x0F00) >> 8] = chip8->delay_timer; // Set VX to the value of the delay timer
                    chip8->PC += 2; // Move to next instruction
                    logInfo("Opcode FX07: Set V[%X] = delay timer value: (%d)", (opcode & 0x0F00) >> 8, chip8->delay_timer);
                    break;

                case 0x00A: { // FX0A : Wait for keypress, store key in VX
                    bool keyPressed = false;
                    for (int i = 0; i < CHIP8_KEYPAD_SIZE; i++) { // Loop through keypad (keys are iterated as value i through the 16 keys)
                        if (chip8->keypad[i]) {
                            chip8->V[(opcode & 0x0F00) >> 8] = i; // Store key in VX
                            keyPressed = true;
                            logInfo("Opcode FX0A: Key pressed: %X pressed, stored in V[%X]", i, (opcode & 0x0F00) >> 8);
                            break;
                        }
                    }

                    if (!keyPressed) {
                        // Do not increment PC to repeat this instruction until a key is pressed
                        return; // Continue to wait for key press
                    }

                    chip8->PC += 2; // Move to next instruction after key press
                    break;
                }

                case 0x0015: // FX15 : Set delay timer to VX 
                    chip8->delay_timer = chip8->V[(opcode & 0xF00) >> 8]; // Set delay timer to VX
                    chip8->PC += 2; // Move to next instruction
                    logInfo("Opcode FX15: Set delay timer to V[%X] = %d", (opcode & 0xF00) >> 8, chip8->V[(opcode & 0xF00) >> 8]);
                    break;
                
                case 0x0018: // FX18 : Set sound timer to VX
                    chip8->sound_timer = chip8->V[(opcode & 0xF00) >> 8]; // Set sound timer to VX
                    chip8->PC += 2; // Move to next instruction
                    logInfo("Opcode FX18: Set sound timer to V[%X] = (%d)", (opcode & 0xF00) >> 8, chip8->V[(opcode & 0xF00) >> 8]);
                    break;
                
                case 0x001E: // FX1E : Add VX to I
                    // notice opcode is FX1E and the case (line 367) is case 0xF000 sp FX1E & 0x00FF = 0x001E
                    chip8->I += chip8->V[(opcode & 0xF000)] >> 8; // Add VX to I
                    chip8->PC += 2; // Move to next instruction
                    logInfo("Opcode FX1E: Set I = I + V[%X] (0x%04X)", (opcode & 0xF00) >> 8, chip8->I);
                    break;

                case 0x0029: //FX29 : Set I = location of sprite for digit VX
                    chip8->I = CHIP8_FONTSET_START_ADDRESS + (chip8->V[(opcode & 0x0F00) >> 8] * 5); // Set I to location of sprite for digit VX
                    chip8->PC += 2; // Move to next instruction
                    logInfo("Opcode FX29: Set I to location of sprite for digit V[%X] = %X", (opcode & 0x0F00) >> 8, chip8->V[(opcode & 0x0F00) >> 8]);
                    break;
            }
            
    }        
}


void updateTimers(chip8_t *chip8) { // Ensures delay timer accurately is updated each cycle (60Hz)
    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }

    if (chip8->sound_timer > 0) {
        chip8->sound_timer--; // Decrement sound timer
        playSound(); // Play sound
        if (chip8->sound_timer == 0) {
            stopSound(); // Stop sound
        }
    }
}

void clearDisplay(chip8_t *chip8) {
    memset(chip8->display, 0, sizeof(chip8->display)); // Clear display
}

bool drawSprite(chip8_t *chip8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height) {
    bool pixelFlipped = false; // Set to true if a pixel is turned off
    int displayWidth = CHIP8_DISPLAY_WIDTH;
    int displayHeight = CHIP8_DISPLAY_HEIGHT;

    for (int row = 0; row < height; row++) {
        uint8_t spriteRow = sprite[row]; // Get current row of sprite
        for (int col = 0; col < 8; col++) {
            uint8_t spritePixel = (spriteRow >> (7 - col)) & 1; // Get current pixel of sprite
            int xPos = (x + col) % displayWidth; // Wrap around if pixel goes off the screen
            int yPos = (y + row) % displayHeight; // Wrap around if pixel goes off the screen
            int pixelIndex = yPos * displayWidth + xPos; // Calculate index of pixel in display
            if (spritePixel) {
                if (chip8->display[pixelIndex]) {
                    pixelFlipped = true; // Collision detected
                }
                chip8->display[pixelIndex] ^= 1; // XOR pixel with sprite pixel
            }
        }
    }
    return pixelFlipped;
}
