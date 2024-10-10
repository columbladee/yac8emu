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

void destroyGraphics() {
	if (texture) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		logInfo("Texture DESTROYED");
	}

	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
		logInfo("Renderer DESTROYED");
	}

	if (window) {
		SDL_DestroyWindow(window);
		window = NULL;
		logInfo("Window DESTROYED");
	}

	SQL_QuitSubSystem(SDL_INIT_VIDEO);
	logInfo("SDL Video subsystem QUIT it just QUIT");
}

void renderGraphics(chip8_t *chip8) {
	//create an array to hold pixel data for the entire display
	uint32_t pixels[CHIP8_DISPLAY_SIZE];

	//Convert display buffer into pixel data for SDL
	for (int i = 0; i < CHIP8_DISPLAY_SIZE; i++) {
		// If pixel is set (1) it is white = 0xFFFFFFFF
		// If pixel is not set (0) it is black 0xFF000000
		pixels[i] = chip8->display[i] ? 0xFFFFFFFF : 0xFF000000;
	}
		
	//	Texture Update - New Pixel Data
	SDL_UpdateTexture(texture, NULL, pixels, CHIP8_DISPLAY_WIDTH * sizeof(uint32_t));
	//	Rendering Process


	SDL_RenderClear(renderer); // Clear current rendering target with 0xFF000000
	SDL_RenderCopy(renderer, texture, NULL, NULL); // Copy texture to render target (render target is the window in this instance)
	SDL_RenderPresent(renderer); // Update screen with rendering performed since last call of renderGraphics
	logDebug("Graphics rendered");

	
}
