#include "chip8.h"
#include "sdl_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// CHIP-8 fontset (contains hexadecimal digits 0-F, stored at memory locations 0x050 to 0x09F)
#define CHIP8_FONTSET_START_ADDRESS 0x50 
#define CHIP8_FONTSET_SIZE 80
#define CHIP8_MEMORY_SIZE 4096 
#define CHIP8_NUM_REGISTERS 16
#define CHIP8_NUM_KEYS 16
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_START_ADDRESS 0x200

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
}

void loadROM(chip8_t *chip8, const char *romPath) {
    FILE *rom = fopen(romPath, "rb");
    if (!rom) {
        fprintf(stderr, "Failed to open ROM: %s\n", romPath);
        exit(1);
    }

    fseek(rom, 0, SEEK_END);
    long romSize = ftell(rom);
    rewind(rom);

    if (romSize > (CHIP8_MEMORY_SIZE - CHIP8_START_ADDRESS)) {
        fprintf(stderr, "ROM too large to fit in memory: %ld bytes\n", romSize);
        exit(1);
    }

    fread(&chip8->memory[CHIP8_START_ADDRESS], sizeof(uint8_t), romSize, rom);
    fclose(rom);
}

uint16_t fetchOpcode(chip8_t *chip8) {
    return (chip8->memory[chip8->PC] << 8) | chip8->memory[chip8->PC + 1];
}

void executeCycle(chip8_t *chip8) {
    chip8->opcode = fetchOpcode(chip8);
    decodeAndExecute(chip8, chip8->opcode);
    updateTimers(chip8);
}

void decodeAndExecute(chip8_t *chip8, uint16_t opcode) {
    // Nibbles:  See documentation

    // Main switch handles high nibble of opcode

    switch (opcode & 0xF000) {

        case 0x0000:  /// 00E0 - Clear the display and 0x00EE through Return from a subroutine
            switch (opcode & 0x00FF) {

                case 0x00E0: // Clear the display
                    clearDisplay(chip8);
                    chip8->drawFlag = true; // Set flag to redraw screen
                    chip8->PC += 2; // Move to next instruction
                    break;

                case 0x00EE: // 00EE Return from a subroutine
                    chip8->SP--; // Decrement stack pointer
                    chip8->PC = chip8->stack[chip8->SP]; // Move to address at top of stack
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    exit(1); 
            }
            break; 

        case 0x1000: // 1NNN - Jump to address NNN 
            chip8->PC = opcode & 0x0FFF; // Set PC to address NNN 
            break;

        case 0x2000: // 2NNN - Call subroutine at NNN
            chip8->stack[chip8->SP] = chip8->PC; // Store current PC on stack
            chip8->SP++; // Increment stack pointer
            chip8->PC = opcode & 0x0FFF; // Jump to subroutine at NNN
            break;
        case 0x3000: // 3XNN - Skip next instruction if VX == NN
            if (chip8->V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                chip8->PC += 4; // Skip the next instruction
            } else {
                chip8->PC += 2; // Move to next instruction
            }
            break;

            case 0x4000: // 4XNN - Skip next instruction if VX != NN
                if (chip8->V[(opcode & 0xF00) >> 8] != (opcode & 0x00FF)) {
                    chip8->PC += 4; // Skip the next instruction
                } else {
                    chip8->PC += 2; // Move to next instruction
                }
                break;
            
            case 0x5000: // 5XY0: Skip next instruction if VX == VY
                if (chip8->V[(opcode & 0x0F00) >> 8] == chip8->V[(opcode & 0x00F0) >> 4]) {
                    chip8->PC += 4; // Skip the next instruction
                } else {
                    chip8->PC += 2; // Move to next instruction
                }
                break;

            case 0x6000: // 6XNN : Set VX to NN
                chip8->V[(opcode & 0x0F00) >> 8] == chip8->V[(opcode & 0x00FF)]; // Set register VX to NN
                chip8->PC += 2; // Move to next instruction
                break;
            
            case 0x7000: // 7XNN : Add NN to VX
                chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF; // Add NN to VX
                chip8->PC +=2; // Move to next instruction
                break;

            case 0x8000: // Handle math and logical operators between registers

                switch (opcode & 0x000F) {
                    case 0x0000: // 8XY0 : Set VX to the value of VY
                        chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0) >> 4]; // VX=VY
                        chip8->PC += 2; // Move to next instruction
                        break;

                        case 0x0001: // 8XY1 : Set VX to bitwise VX OR VY
                            chip8->V[(opcode & 0x0F00) >> 8] |= chip8->V[(opcode & 0x0F00) >> 4]; // VX |= VY
                            chip8->PC += 2; // Move to next instruction
                            break;
                        
                        case 0x0002: // 8XY2 : Set VX to bitwise VX AND VY
                            chip8->V[(opcode & 0x0F00) >> 8] &= chip8->V[(opcode & 0x0F00) >> 4]; // VX |= VY
                            chip8->PC += 2; // Move to next instruction
                            break;

                        case 0x0003: // 8XY2 : Set VX to bitwise VX XOR VY
                            chip8->V[(opcode & 0x0F00) >> 8] ^= chip8->V[(opcode & 0x0F00) >> 4]; // VX |= VY
                            chip8->PC += 2; // Move to next instruction
                            break;

                        case 0x0004: // 8XY4: Add VY to VX, set VF on carry
                            if (chip8->V[(opcode & 0x00F0) >> 4] > (0xFF - chip8->V[(opcode & 0xF00) >> 8])) {
                                chip8->V[0xF] = 1; // Carry flag set to 1 if there is an overflow
                            } else {
                                chip8->V[0xF] = 0; // No carry
                            }
                            break;
                        
                        case 0x0005:  // 8XY5 : Subtract VY from VX, set VF if no borrow
                            // some turmoil occured here
                            if (chip8->V[(opcode & 0x00F0) >> 4] > chip8->V[(opcode & 0x0F00 >> 8)]) {
                                chip8->V[0xF] = 0; // Borrow occured
                            } else {
                                chip8->V[0xF] = 1; // No borrow
                            }
                            chip8->V[(opcode & 0x0F00) >> 8] -= chip8->V[(opcode & 0x00F0) >> 4]; //VX -= VY
                            chip8->PC += 2; // Move to next instruction
                            break;
                        break;

                    case 0x0006: // 8XY6 Store least significant bit of VX in VF, then shift VX to the right by 1
                        chip8->V[0xF] = chip8->V[(opcode & 0x0F00) >> 8] & 0x1; // Store least significant bit of VX in VF
                        chip8->V[(opcode & 0x0F00) >> 8] >>= 1; // Shift VX to the right by 1
                        chip8->PC += 2; // Move to next instruction
                        break;
                    
                    case 0x0007: // 8XY7 : Set VX to VY - VX, set VF if no borrow
                        if (chip8->V[(opcode & 0x0F00) >> 8] > chip8->V[(opcode & 0x00F0) >> 4]) {
                            chip8->V[0xF] = 0; // Borrow occured
                        } else {
                            chip8->V[0xF] = 1; // No borrow
                        }
                        chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0) >> 4] - chip8->V[(opcode & 0x0F00) >> 8]; // VX = VY - VX
                        chip8->PC += 2; // Move to next instruction
                        break;
                    
                    case 0x000E: // 8XYE: Store most significant bit of VX in VF, then shift VX to the left by 1
                        chip8->V[0xF] = chip8->V[(opcode & 0x0F00) >> 8] >> 7; // Store most significant bit of VX in VF
                        chip8->V[(opcode & 0x0F00) >> 8] <<= 1; // Shift VX to the left by 1
                        chip8->PC += 2; // Move to next instruction
                        break;
                    
                    default:
                        printf("Unknown opcode: %X\n", opcode);
                        exit(1);
                        break;
                }
                break;
            
            case 0xA000: // ANNN : Set I to address NNN
                chip8->I = opcode & 0x0FFF; // Set I to address NNN
                chip8->PC += 2; // Move to next instruction
                break;
            
            case 0xC000: // CXXN: Set VX to random byte AND NN
                chip8->V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF); // VX = random byte AND NN
                chip8->PC += 2;
                break;
            
            case 0xD000: {  // DXYN : Displays N-byte sprite starting at memory address I at (VX, VY), set VF = collision
                uint8_t x = chip8->V[(opcode & 0xF00) >> 8]; // X-coordinate from VX
                uint8_t y = chip8->V[(opcode & 0x0F0) >> 4]; // Y-coordinate from VY
                uint8_t height = opcode & 0x000F; // Height (N bytes) from the last nibble
                const uint8_t *sprite = opcode &chip8->memory[chip8->I]; // Sprite draw starting at memory address I
                chip8->V[0xF] = drawSprite(chip8, x, y, sprite, height) ? 1 : 0; // Set VF to 1 if collision 
                chip8->drawFlag = true; // Set draw flag to redraw screen
                chip8->PC += 2; // Move to next instruction
            }
            break;

            case 0xE000:
                // Opcodes below handle key press events
                switch (opcode & 0x00FF) {
                    case 0x009E: // EX9E : Skip next instruction if key stored in VX is pressed
                        if (chip8->keypad[chip8->V[(opcode & 0x0F00) >> 8 ]]) {
                            chip8->PC += 4; // skip next instruction
                        } else {
                            chip8->PC += 2; // Move to next instruction
                        }
                        break;
                    
                    case 0x00A1: //EXA1 : Skip next instruction if key stored in VX is not pressed
                        if (!chip8->keypad[chip8->V[(opcode & 0x0F00) >> 8]]) {
                            chip8->PC += 4; // Skip next instruction
                        } else {
                            chip8->PC += 2; // Move to next instruction
                        }
                        break;

                    default:
                        printf("Unknown opcode: 0x%X\n", opcode);
                        exit(1); 
                }
                break;
            
            case 0xF000:
            // Handle timers, memory, input
            switch (opcode & 0x00FF) {
                case 0x0007: // FX07 : Set VX to value of delay timer
                    chip8->V[(opcode & 0x0F00) >> 8] = chip8->delay_timer; // Set VX to value of delay timer
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                case 0x000A: // FX0A : Wait for key press, store key in VX
                {
                    bool keyPressed = false;
                    for (int i = 0; i < CHIP8_KEYPAD_SIZE; i++) {
                        if (chip8->keypad[i]) {
                            chip8->V[(opcode & 0x0F00) >> 8] = i; // Store key in VX
                            keyPressed = true;
                            break;
                        }
                    }

                    if (!keyPressed) {
                        return; // Keep waiting for key press
                    }

                    chip8->PC += 2; // Move to next instruction after key press
                }
                break;

                case  0x0015: // FX15 : Set delay timer to VX
                    chip8->delay_timer = chip8->V[(opcode & 0x0F00) >> 8]; // Set delay timer to VX
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                case 0x0018: // FX18 : Set sound timer to VX
                    chip8->sound_timer = chip8->V[(opcode & 0x0F00) >> 8]; // Set sound timer to VX
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                case 0x001E: // FX1E : Add VX to I
                    chip8->I += chip8->V[(opcode & 0x0F00) >> 8]; // Add VX to I
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                case 0x0033: // FX33 : Store BCD representation of VX in memory locations I, I+1, I+2
                    chip8->memory[chip8->I] = chip8->V[(opcode & 0x0F00) >> 8] / 100; // Hundreds digit
                    chip8->memory[chip8->I + 1] = (chip8->V[(opcode & 0x0F00) >> 8] / 10) % 10; // Tens digit
                    chip8->memory[chip8->I + 2] = chip8->V[(opcode & 0x0F00) >> 8] % 10; // Ones digit
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                case 0x0055: // FX55: Store registers V0 through VX in memory starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        chip8->memory[chip8->I + i] = chip8->V[i]; // Store V0 to VX in memory starting at address I
                    }
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                case 0x0065: // FX65 : Read registers V0 through VX from memory starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        chip8->V[i] = chip8->memory[chip8->I + i]; // Load memory into V0 to VX
                    }
                    chip8->PC += 2; // Move to next instruction
                    break;
                
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    exit(1);
            }
            break;

            default:
                printf("Unknown opcode: 0x%X\n", opcode);
                exit(1);
    }

}


void updateTimers(chip8_t *chip8) { // Ensures delay timer accurately is updated each cycle (60Hz)
    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }

    if (chip8->sound_timer > 0) {
        if (--chip8->sound_timer == 0) {
            // Play sound when timer hits zero (handled by SDL)
            playSound(); 
        }
    }
}

void clearDisplay(chip8_t *chip8) {
    memset(chip8->display, 0, sizeof(chip8->display)); // Clear display
}

bool drawSprite(chip8_t *chip8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height) {
    bool pixelFlipped = false; // Set to true if a pixel is turned off

    for (int row = 0; row < height; row++) {
        uint8_t spriteRow = sprite[row]; // Get current row of sprite
        for (int col = 0; col < 8; col++) {
            uint8_t spritePixel = (spriteRow & (0x80 >> col)) >> (7 - col); // Get current pixel of sprite
            int displayIndex = (x + col) % CHIP8_DISPLAY_WIDTH + ((y + row) % CHIP8_DISPLAY_HEIGHT) * CHIP8_DISPLAY_WIDTH; // Get index of display pixel

            if (spritePixel) {
                if (chip8->display[displayIndex]) {
                    pixelFlipped = true; // Collision detected
                }
                chip8->display[displayIndex] ^= 1; // XOR with sprite pixel
            }
        }
    }

    return pixelFlipped;
}
