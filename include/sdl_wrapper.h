#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

#include <SDL2/SDL.h>
#include "chip8.h"

int initializeSDL(chip8_t *chip8);

void destroySDL();

void renderGraphics(chip8_t *chip8);

void handleSDLInput(chip8_t *chip8);

void playSound();

void stopSound();

#endif // SDL_WRAPPER_H