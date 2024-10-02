#include "sdl_wrapper.h"
#include <stdio.h>

//Scaled up from chip8 display width (64) and height (32)
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320
#define PIXEL_SIZE 10

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_AudioDeviceID audioDevice;
static SDL_bool isAudioOpened = SDL_FALSE;

//Initialize SDL, grapics window and audio system

int initializeSDL(chip8_t *chip8) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Audio
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 800; // Frequency of the sound
    desiredSpec.format = AUDIO_F32SYS; // Lots of audio formats to choose
    desiredSpec.channels = 1; // Mono sound
    desiredSpec.samples = 512; // Good default buffer size
    desiredSpec.callback = NULL; // No callback , will use SDL_QueueAudio

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, NULL, 0);
    if (!audioDevice) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        return 1;
    } else {
        isAudioOpened = SDL_TRUE;
    }

    return 0;
}

// SDL Input (keypad) handling
void handleSDLInput(chip8_t *chip8) {

}

void renderGraphics(chip8_t *chip8) {

}

void playSound() {

}

void stopSound() {

}

void destroySDL() {

}
