#ifndef MEMORY_H
#define MEMORY_H

#include "chip8.h"

void loadROM(chip8_t *chip8, const char *romPath);
void initializeMemory(chip8_t *chip8);

#endif //MEMORY_H
