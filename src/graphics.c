#include "graphics.h"
#include "logger.h"
#include <SDL2/SDL.h>

//Constants for window size

#define WINDOW_SCALE 10
#define WINDOW_WIDTH (CHIP8_DISPLAY_WIDTH * WINDOW_SCALE)
#define WINDOW_HEIGHT (CHIP8_DISPLAY_HEIGHT * WINDOW SCALE)

//SDL Vars


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

int initializeGraphics(chip8_t *chip8) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		logError("Can't start SDL video, error: %s", SDL_GetError(());
		return -1;
	}
	logInfo("SDL Video initialized");

	window = SDL_CreateWindow("CHIP-8 Emulator",
				  SDL_WINDOWPOS_CENTERED,
				  SDL_WINDOWPOS_CENTERED,
				  WINDOW_WIDTH,
				  WINDOW_HEIGHT,
				  SDL_WINDOW_SHOWN);
	if (!window) {
		logError("Failed to create window with error %s", SDL_GetError(());
		return -1;
	}
	logInfo("Window created");

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		logError("Failed to create renderer with error: %s", SDL_GetError(());
		return -1;
	}
	logInfo("Renderer Created");

	texture = SDL_CreateTexture(renderer,
				    SDL_PIXELFORMAT_RGBA8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    CHIP8_DISPLAY_WIDTH,
				    CHIP8_DISPLAY_HEIGHT);
	if (!texture) {
		logError("Failed to create texture with error: %s", SDL_GetError(());
		return -1;
	}
	logInfo("Texture created");

	return 0;

}
