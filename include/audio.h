//audio.h


#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

void initializeAudio();
void playsSound();
void stopSound();
void cleanupAudio();
bool isSoundPlaying();

#endif // AUDIO_H
