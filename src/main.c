include "chip8.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "logger.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: %s <ROM_FILE>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	// Init logger, SDL, Graphics, Audio, CHIP-8 instance

	initializeLogger("logs/chip8_emulator.log");
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

	initializeAudio();

	chip8_t chip8;
	initializeCPU(&chip8)
	loadROM(&chip8, argv[1]);
	

	// Main emulation loop


}
