// audio.c 

#include "audio.h"
#include "logger.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <string.h> 
#include <stdint.h> // for int16_t
#include <limits.h> // for INT16_MAX for AUDIO_S16SYS


#define SAMPLE_RATE 44100
// Frequency is tone in Hz
#define FREQUENCY 800.0f
// Amplitude of wave (0.0f to 1.0f)
#define AMPLITUDE 0.5f

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static SDL_AudioDeviceID audioDevice = 0;
static bool isPlaying = false;
static float phase = 0.0f; // Phase accumulator
			   
void audioCallback(void *userdata, uint8_t *stream, int len) {
	(void)userdata;
	int16_t *buffer = (int16_t *)stream;
	int samples = len / sizeof(int16_t);

	double phaseIncrement = (2.0 * M_PI * FREQUENCY) / SAMPLE_RATE;
	for (int i = 0; i < samples; i++) {
		//Generate a square wave using the sign of sine function
		float sampleValue = (sin(phase) > 0.0) ? AMPLITUDE : -AMPLITUDE;
		buffer[i] = sampleValue;
		phase += phaseIncrement;

		//Wrap phase to [0, 2PI) to avoid overflow
		if (phase >= 2.0 * M_PI) {
			phase -= 2.0 * M_PI;
		}
	}
}

int initializeAudio() {
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		logError("Failed to initialize SDL audio: %s", SDL_GetError());
		return;
	} 

	// Declare desiredSpec before use - was a problem before
	// https://wiki.libsdl.org/SDL2/SDL_OpenAudio
	SDL_AudioSpec desiredSpec;
	//void *memset(void *str, int c, size_t n)
	memset(&desiredSpec, 0, sizeof(desiredSpec)); // Initialize desiredSpec to 0

	desiredSpec.freq = SAMPLE_RATE;
	desiredSpec.format = AUDIO_S16SYS; // 32-bit float samples
	desiredSpec.channels = 1; // Mono sound
	desiredSpec.samples = 4096; // Good default buffer size
	desiredSpec.callback = audioCallback;

	// Open audio device

	audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, NULL, 0);
	if (audioDevice == 0) {
		logError("Failed to open audio device: %s", SDL_GetError());
		return;
	}

	return 0;
	logInfo("Audio device OPENED!");
}

void playSound() {
	if (audioDevice == 0) {
		logWarning("Audio device not initialized!");
		return;
	}
	if (!isPlaying) {
		SDL_PauseAudioDevice(audioDevice, 0);
		isPlaying = true;
	}
}

void stopSound() {
	if (audioDevice == 0) {
		logWarning("Audio device not initialized!");
		return;
	}
	if (isPlaying) {
		SDL_PauseAudioDevice(audioDevice, 1);
		isPlaying = false;
		logDebug("Sound stopped!");
	}
}

void cleanupAudio() {
	if (audioDevice != 0) {
		// https://wiki.libsdl.org/SDL2/SDL_CloseAudio
		SDL_CloseAudioDevice(audioDevice);
		audioDevice = 0;
		logInfo("Audio device CLOSED!");
	}
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	logInfo("Audio cleaned up!");
}

bool isSoundPlaying() {
	return isPlaying;
}
