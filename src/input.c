#include "chip8.h"
#include "input.h"
#include "logger.h"
#include <SDL2/SDL.h>

//Map SDL key events to CHIP-8 key inputs


			//Map SDL Keys to corresponding Chip-8 keypad keys
			//Left: Keyboard Right: CHIP-8 Keypad
			// 1 2 3 4 -> 1 2 3 C
			// Q W E R -> 4 5 6 D
			// A S D F -> 7 8 9 E
			// Z X C V -> A 0 B F

void handleInput(chip8_t *chip8, bool *running) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SQL_QUIT:
				logInfo("SDL_QUIT event detected");
				*running = false;
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP: {
				bool keyState = (event.type == SDL_KEYDOWN);
				uint8_t key = 0xFF;
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: // Exit emulator
						*running = false;
						break;
					case SDLK_1: key = 0x1; break;
					case SDLK_2: key = 0x2; break;
					case SDLK_3: key = 0x3; break;
					case SDLK_4: key = 0xC; break;
					case SDLK_q: key = 0x4; break;
					case SDLK_w: key = 0x5; break;
					case SDLK_e: key = 0x6; break;
					case SDLK_r: key = 0xD; break;
					case SDLK_a: key = 0x7; break;
					case SDLK_s: key = 0x8; break;
					case SDLK_d: key = 0x9; break;
					case SDLK_f: key = 0xE; break;
					case SDLK_z: key = 0xA; break;
					case SDLK_x: key = 0x0; break;
					case SDLK_c: key = 0xB; break;
					case SDLK_v: key = 0xF; break;
					default:
						break;
				}
				if (key != 0xFF) {
					chip8->keypad[key] = keyState; // Set key state , in other words if key is pressed or not
					logDebug("Key %X %s", key, keyState ? "pressed" : "released");
				}
				break;
			}
			default: 
				break;
		}
	}
}