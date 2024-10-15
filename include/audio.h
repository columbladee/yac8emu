//audio.h


#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

int initializeAudio();
void playsSound();
void stopSound();
void cleanupAudio();
bool isSoundPlaying();

#endif // AUDIO_H
