#include "chip8.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "logger.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void cleanup() {	
	destroyGraphics();
	cleanupAudio();
	destroySDL();
	closeLogger();
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: %s <ROM_FILE>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	// Init logger, SDL, Graphics, Audio, CHIP-8 instance

	initLogger("logs/chip8_emulator.log");
	logInfo("CHIP-8 Emulator started");

	if (initializeSDL() != 0) {
		logError("Failed to initialize SDL");
		closeLogger();
		return EXIT_FAILURE;
	}

	if (initializeGraphics() != 0) {
		logError("Failed to initialize graphics");
		destroySDL();
		closeLogger();
		return EXIT_FAILURE;
	}

	if (initializeAudio() != 0) {
		logError("Failed to initialize audio");
		destroyGraphics();
		destroySDL();
		closeLogger();
		return EXIT_FAILURE;
	}

	//Create CHIP8 instance
	chip8_t chip8;
	initializeCPU(&chip8);

	// Load ROM
	if (loadROM(&chip8, argv[1]) != 0);
		logError("Failed to load ROM");
		cleanup();
		return EXIT_FAILURE;
	}
		
	// Main emulation loop
	
	bool running = true;
	uint32_t lastCycleTime = SDL_GetTicks();
	while (running) {
		handleInput(&chip8, &running);

		//One cycle
		executeCycle(&chip8);

		//Render if needed
		
		if (chip8.drawFlag) {
			renderGraphics(&chip8);
			chip8.drawFlag = false;
		}

		// Delay to control emulation speed
		// This can be made more accurate/done better

		uint32_t currentTime = SDL_GetTicks();
		uint32_t elapsedTime = currentTime - lastCycleTime;
		if (elapsedTime < 2 ) {
			SDL_Delay(2 - elapsedTime);
		}
		lastCycleTime = currentTime;
	}
	
	//Cleanup before exiting
	cleanup();

	logInfo("CHIP-8 Emulator was nicely terminated");
	return EXIT_SUCCESS;
}
