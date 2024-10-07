#include "chip8.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>

//sdl_wrapper.c:41:    desiredSpec.freq = 800; // Frequency of the sound

//Audio frequency and format settings

/* from sdl_wrapper.c  -> clearly I decided to ignore using this because I'm repeating myself all over the place  
SDL_AudioSpec desiredSpec;
SDL_zero(desiredSpec);
desiredSpec.freq = 800; // Frequency of the sound
desiredSpec.format = AUDIO_F32SYS; // Lots of audio formats to choose
desiredSpec.channels = 1; // Mono sound
desiredSpec.samples = 512; // Good default buffer size
desiredSpec.callback = NULL; // No callback , will use SDL_Queue
*/

#define AUDIO_FREQUENCY  800
#define AUDIO_FORMAT AUDIO_F32SYS
#define AUDIO_CHANNELS 1
#define AUDIO_SAMPLES 512

// Function to initialize audio system

int initializeAudio() {
	//Define desired audio settings
	SDL_AudioSpec desiredSpec; 
}

// Please hold, making development branch, to not repeat self
//

