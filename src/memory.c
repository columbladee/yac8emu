#include "chip8.h"
#include <stdio.h>
#include <string.h>


#define CHIP8_MEMORY_SIZE 4096          
#define CHIP8_FONTSET_START_ADDRESS 0x50 
#define CHIP8_FONTSET_SIZE 80
#define CHIP8_NUM_REGISTERS 16          
#define CHIP8_NUM_KEYS 16               
#define CHIP8_DISPLAY_WIDTH 64          
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

void initializeMemory(chip8_t *chip8) {
    memset(chip8->memory, 0, sizeof(chip8->memory)); // Clear memory

    //Load fontset into memory (starting at 0x50)
    for (int i = 0; i < CHIP8_FONTSET_SIZE; i++) {
        chip8->memory[CHIP8_FONTSET_START_ADDRESS + i] = chip8_fontset[i]; 
    }
}

int loadROM(chip8_t *chip, const char *romPath) {
    FILE *rom = fopen(romPath, "rb");
    if (!rom) {
        fprintf(stderr, "Failed to open ROM: %s\n", romPath);
        return 1;
    }

    //Get size of ROM
    fseek(rom, 0, SEEK_END); // Seek to end of file
    long romSize = ftell(rom); 
    rewind(rom); // Reset file pointer to beginning of file

    // Can the ROM fit in memory? (Max size after 0x200)
    if (romSize > (CHIP8_MEMORY_SIZE - CHIP8_START_ADDRESS)) {
        fprintf(stderr, "ROM too large to fit in memory: %ld bytes\n", romSize);
        return 1;
    }

    //Load ROM into memory (starting at 0x200)
    fread(&chip->memory[CHIP8_START_ADDRESS], sizeof(uint8_t), romSize, rom);
    fclose(rom);

    return 0;
}
