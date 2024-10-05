#include "chip8.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>

//Map SDL key events to CHIP-8 key inputs

void handleSDLInput(chip8_t *chip8)  {
	SDL_Event event;

	//Poll for events, then process each one
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			exit(0);
		}

		if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
			//Check whether keypress or key release
			uint8_t value = (event.type == SDL_KEYDOWN) ? 1 : 0;

			//Map SDL Keys to corresponding Chip-8 keypad keys
			//Left: Keyboard Right: CHIP-8 Keypad
			// 1 2 3 4 -> 1 2 3 C
			// Q W E R -> 4 5 6 D
			// A S D F -> 7 8 9 E
			// Z X C V -> A 0 B F
			
			switch (event.key.keysym.sym) {
				case SDLK_1: chip8->keypad[0x1] = value; break;
				case SDLK_2: chip8->keypad[0x2] = value; break;
				case SDLK_3: chip8->keypad[0x3] = value; break;
				case SDLK_4: chip8->keypad[0xC] = value; break;

				case SDLK_q: chip8->keypad[0x4] = value; break;
				case SDLK_w: chip8->keypad[0x5] = value; break;
				case SDLK_e: chip8->keypad[0x6] = value; break;
				case SDLK_r: chip8->keypad[0xD] = value; break;

				case SDLK_a: chip8->keypad[0x7] = value; break;
				case SDLK_s: chip8->keypad[0x8] = value; break;
				case SDLK_d: chip8->keypad[0x9] = value; break;
				case SDLK_f: chip8->keypad[0xE] = value; break;

				case SDLK_z: chip8->keypad[0xA] = value; break;
				case SDLK_x: chip8->keypad[0x0] = value; break;
				case SDLK_c: chip8->keypad[0xB] = value; break;
				case SDLK_v: chip8->keypad[0xF] = value; break;
				
				default: break;
			}
		}
	}
}

//Check if key is pressed in the chip-8 keypad
bool isKeyPressed(chip8_t *chip8, uint8_t key) {
	//Return true if keypad pressed, otherwise false
	return chip8->keypad[key]
}

uint8_t waitForKeyPress(chip8_t *chip8) {
	SDL_Event event;
	while(1) {
		//Poll for events (while blocking call)
		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_QUIT) {
				exit(0);
			}
			if (event.type == SDL_KEYDOWN) {
				//Map SDL keys to chip 8 keys ... again
				switch (event.key.keysym.sym) {
					case SDLK_1: return 0x1;
					case SDLK_2: return 0x2;
					case SDLK_3: return 0x3;
					case SDLK_4: return 0xC;

					case SDLK_q: return 0x4;
					case SDLK_w: return 0x5;
					case SDLK_e: return 0x6;
					case SDLK_r: return 0xD;

					case SDLK_a: return 0x7;
					case SDLK_s: return 0x8;
					case SDLK_d: return 0x9;
					case SDLK_f: return 0xE;

					case SDLK_z: return 0xA;
					case SDLK_x: return 0x0;
					case SDLK_c: return 0xB;
					case SDLK_v: return 0xF;
					
					default: break;
				}	
			}
		}
	}
}
