#include "sdl_wrapper.h"
#include "logger.h"
#include <stdio.h>
#include <SDL2/SDL.h>

// https://wiki.libsdl.org/SDL2/APIByCategory
// https://wiki.libsdl.org/SDL2/CategoryAPIFunction
// https://wiki.libsdl.org/SDL2/CategoryAPIStruct



int initializeSDL() { 
    if (SDL_Init(0) != 0) {
        logError("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    logInfo("SDL initialized successfully");
    return 0;
}

void destroySDL() {
    SDL_Quit();
    logInfo("SDL destroyed successfully");
}

