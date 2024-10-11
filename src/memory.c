#include "chip8.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
