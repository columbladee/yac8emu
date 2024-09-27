#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHIP8_FONTSET_START_ADDRESS 0x50 // CHIP-8 fontset (contains hexadecimal digits 0-F, stored at memory locations 0x050 to 0x09F)
#define CHIP8_FONTSET_SIZE 80
#define CHIP8_MEMORY_SIZE 4096 
#define CHIP8_NUM_REGISTERS 16
#define CHIP8_NUM_KEYS 16
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_START_ADDRESS 0x200

// CHIP-8 fontset (contains hexadecimal digits 0-F, stored at memory locations 0x050 to 0x09F)

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
    // I actually forgot... -> is for accessing members of a struct (or union) through a pointer
    /*
    in the cases below... chip8 variable is a pointer to a chip8_t struct
    chip8 struct from chip8.h  Allows for access to the array (V, stack, memory, display, keypad)
    Witout -> it would be
    (*chip8).variable 
    or 
    (*chip8).arrayName[index]
    
    
    */
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
    }

}

void updateTimers(chip8_t *chip8) {
}


void clearDisplay(chip8_t *chip8) {
}

bool drawSprite(chip8_t *chip8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height) {
}