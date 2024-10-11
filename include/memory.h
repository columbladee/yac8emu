#ifndef MEMORY_H
#define MEMORY_H

#include "chip8.h"

void initializeMemory(chip8_t *chip8);
void loadROM(chip8_t *chip8, const char *romPath);


#endif //MEMORY_H
