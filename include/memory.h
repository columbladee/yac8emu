#ifndef MEMORY_H
#define MEMORY_H

#include "chip8.h"

int initializeMemory(chip8_t *chip8);
int loadROM(chip8_t *chip8, const char *romPath);


#endif //MEMORY_H
