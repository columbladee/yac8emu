// audio.c 

#include "audio.h"
#include "logger.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

// NYI : Make this a square wave , not a sine wave, for accuracy
/*
Pseudocode for square wave

function generateSquareWave(amplitude, frequency, duration):
    // Initialize variables
    period = 1 / frequency
    currentTime = 0 
    outputValue = 0 

    while currentTime < duration:
        // Check if current time is within the high period
        if currentTime < (period / 2):
            outputValue = amplitude
        else:
            outputValue = -amplitude 
        
        // Update time
        currentTime += timeStep 
        
        // Output the value
        print(outputValue) 

    return 


	Condition check: Inside the loop, an if statement checks if the current time is within the first half of the period (currentTime < (period / 2)).

    If true, the outputValue is set to the positive amplitude.
    If false, the outputValue is set to the negative amplitude.

*/

#define SAMPLE_RATE 44100
#define FREQUENCY 800.0f

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static SDL_AudioDeviceID audioDevice = 0;
static bool isPlaying = false;
static float phase = 0.0f; // Phase of sine wave
			   
//Audio callback

// If you don't already know basic wave math: http://basicsynth.com/index.php?page=basic

void audioCallback(void *userdata, uint8_t *stream, int len) {
	float *buffer = (float *)stream;
	int samples = len / sizeof(float);

	float amplitude = 0.5f; // Adjust volume (0.0f to 1.0f)
	float phaseIncrement (2.0f * (float)M_PI * FREQUENCY) / SAMPLE_RATE;

	for (int i = 0; i < samples; i++) {
		buffer[i] = amplitude * sinf(phase);
		phase += phaseIncrement; 
		if (phase >= 2.0f * (float)M_PI) {
			phase -= 2.0f * (float)M_PI;
		}
	}	
}

void initializeAudio() {
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0 ) {
		logError("Unable to initialize SDL Audio: %s", SDL_GetError()); 
		return;
	}

	SDL_Audiospec desiredSpec;
	SDL_zero(desiredSpec);
	desiredSpec.freq = SAMPLE_RATE;
	desiredSpec.format = AUDIO_F32SYS;
	desiredSpec.channels = 1; // mono
	desiredSpec.samples = 1024; // buffer size
	desiredSpec.callback = audioCallback;

	SDL_Audiospec obtainedSpec;
	audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
	if (audioDevice == 0) {
		logError("Failed to open audio device with error %s", SDL_GetError());
		return;
	}

	logInfo("Audio initialized with device ID %d", audioDevice)
}


void playSound() {
	if (audioDevice == 0) {
		logWarning("Audio device not initialized!");
		return;
	}

	if (!isPlaying) {
		SDL_PauseAudioDevice(audioDevice, 0); // start playback
		isPlaying = true;
		logDebug("Sound playback started");
	}
}

void stopSound() {
	if (audioDevice == 0) {
		logWarning("Audio device not initialized!");
		return;
	}
	
	if (isPlaying) {
		SDL_PauseAudioDevice(audioDevice, 1); // Pause Playback
		isPlaying = false;
		logDebug("Sound playback stopped");
	}
}


void cleanupAudio() {
	if (audioDevice != 0) {
		SDL_CloseAudioDevice(audioDevice);
		audioDevice = 0;
		logInfo("Audio device IS NO MORE");
	}

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	logInfo("Audio subsystem is clean and tidy");
}

bool isSoundPlaying() {
	return isPlaying;
}
