#include "sdl_wrapper.h"
#include "logger.h"
#include <SDL2/SDL.h>

int initializeSDL() {
	if (SDL_INIT(0) != 0 ) {
		logError("Unable to inistialize SDL: $s", SDL_GETERROR());
		return -1
	}
	logInfo("SDL Initialized");
	return 0	
}

void destroySDL() {
	SQL_QUIT();
	logIngo"SDL Quit"
}
