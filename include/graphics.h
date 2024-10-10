#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "chip8.h"

int initializeGraphics();
void destroyGraphics();
void renderGraphics(chip8_t *chip8);

#endif // GRAPHICS_H
