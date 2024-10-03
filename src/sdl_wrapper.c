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
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT ) {
            exit(0);
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            // Map keyboard keys to CHIP-8 keypad
            uint8_t value = (event.type == SDL_KEYDOWN) ? 1 : 0;
            switch (event.key.keysym.sym) {
                case SDLK_1: chip8->keypad[0x1] = value; break; // 1
                case SDLK_2: chip8->keypad[0x2] = value; break; // 2
                case SDLK_3: chip8->keypad[0x3] = value; break; // 3
                case SDLK_4: chip8->keypad[0xC] = value; break; // 4
                case SDLK_q: chip8->keypad[0x4] = value; break; // Q
                case SDLK_w: chip8->keypad[0x5] = value; break; // W
                case SDLK_e: chip8->keypad[0x6] = value; break; // E
                case SDLK_r: chip8->keypad[0xD] = value; break; // R
                case SDLK_a: chip8->keypad[0x7] = value; break; // A
                case SDLK_s: chip8->keypad[0x8] = value; break; // S
                case SDLK_d: chip8->keypad[0x9] = value; break; // D
                case SDLK_f: chip8->keypad[0xE] = value; break; // F
                case SDLK_z: chip8->keypad[0xA] = value; break; // Z
                case SDLK_x: chip8->keypad[0x0] = value; break; // X
                case SDLK_c: chip8->keypad[0xB] = value; break; // C
                case SDLK_v: chip8->keypad[0xF] = value; break; // V
                default: break;
            }
        }
    }
}

void renderGraphics(chip8_t *chip8) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set black background
    SDL_RenderClear(renderer); // Clear the screen

    //Set color for drawing pixels (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
            if (chip8->display[y * CHIP8_DISPLAY_WIDTH + x]) {
                //Draw a filled rectangle for each pixel that is set
                SDL_Rect rect;
                rect.x = x * PIXEL_SIZE;
                rect.y = y * PIXEL_SIZE;
                rect.w = PIXEL_SIZE;
                rect.h = PIXEL_SIZE;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer); // Update the screen
}

void playSound() {
    if (isAudioOpened) {
        // Generate square wave for buzzer
        float squareWave[800];
        for (int i = 0; i < 800; i++){
            // Generate a square wave: 0.5 for the first half of the period, -0.5 for the second half
            squareWave[i] = (i % 100 < 50) ? 0.5f : -0.5f;
        }
        SDL_QueueAudio(audioDevice, squareWave, sizeof(squareWave));
        SDL_PauseAudioDevice(audioDevice, 0); // Start playing sound
    }
}

void stopSound() {
    if (isAudioOpened) {
        SDL_ClearQueuedAudio(audioDevice); // Stop any sound that is queued
        SDL_PauseAudioDevice(audioDevice, 1); // Stop playing sound
    }
}

void destroySDL() {
    if (isAudioOpened) {
        SDL_CloseAudioDevice(audioDevice);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
