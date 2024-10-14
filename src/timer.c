#include "chip8.h"
#include "audio.h"
#include "sdl_wrapper.h" //for playSound()

//Function to update the delay and sound timers
void updateTimers(chip8_t *chip8) {
	// Decrement delay timer if > 0
	if (chip8->delay_timer > 0) {
		chip8->delay_timer--;
	}

	// Decrement sound timer if > 0 and playSound() if = 0
	if (chip8->sound_timer > 0) {
		chip8->sound_timer--;
	}
	if (chip8->sound_timer == 0) {
		playSound();
	}
}
