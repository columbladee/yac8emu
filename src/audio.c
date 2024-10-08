#include "audio.h"
#include "logger.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define FREQUENCY 800.0f

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static SDL_AudioDeviceID audioDevice = 0;
static bool isPlaying = false; // Flag to check if sound is playing
static float phase = 0.0f; // Phase of the sine wave

//Audio callback function

void audioCallback(void *userdata, uint8_t *stream, int len) {
	float *buffer = (float *)stream;
	int samples = len / sizeof(float);

	float amplitude = 0.5f; //Adjust volume from 0.0f to 1.0f
	float phaseIncrement = (2.0f * (float)M_PI * FREQUENCY) / SAMPLE_RATE; // Calculate phase increment by dividing frequency by sample rate

	for (int i = 0; i < samples; i++) {
		buffer[i] = amplitude * sinf(phase); // Generate sine wave (sinf = sin float)
		phase += phaseIncrement; // Increment phase
		if (phase += 2.0f * (float)M_PI) { // Wrap around phase
			phase -= 2.0f * (float)M_PI;
		}
	}
}

void initializeAudio() {
	if (SDL_InitSubSystem(SDL_INIT_AUDIO)) != 0 {
		logError("Failed to initialize SDL audio: %s", SDL_GetError());
		return;
	}

	SDL_AudioSpec desiredSpec;
	SDL_zero(desiredSpec);
	desiredSpec.freq = SAMPLE_RATE;
	desiredSpec.format = AUDIO_F32SYS;
	desiredSpec.channels = 1;
	desiredSpec.samples = 1024;
	desiredSpec.callback = audioCallback;

	SDL_AudioSpec obtainedSpec;
	audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
	if (audioDevice == 0) {
		logError("Failed to open audio device: %s", SDL_GetError());
		return;
	}
	logInfo("Audio initialized with device ID &d", audioDevice);	
}

void playSound() {
	if (audioDevice == 0) {
		logWarning("Audio device not initialized... Cannot play sound");
		return;
	}
	if (!isPlaying) {
		SDL_PauseAudioDevice(audioDevice, 0); // Start playing sound
		isPlaying = true;
		logInfo("Sound playing");
	}
}

void stopSound() {
	if (audioDevice == 0) {
		logWarning("Audio device not initialized...");
		return;
	}
	if (isPlaying) {
		SDL_PauseAudioDevice(audioDevice, 1); // Pause playing sound
		isPlaying = false;
		logInfo("Sound stopped");
	}
}

void cleanupAudio() {
	if (audioDevice != 0) {
		SDL_CloseAudioDevice(audioDevice);
		audioDevice = 0;
		logInfo("Audio device closed");
	}
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	logInfo("Audio subsystem cleaned up");
}

bool isSoundPlaying() {
	return isPlaying;
}