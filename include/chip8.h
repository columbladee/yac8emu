#ifndef CHIP8_H
#define CHIP8_H


#include <stdint.h>
#include <stdbool.h>

// Hardware Constants
// 4Kb memory (0x000-0xFFF , 0x000-0x1FF is reserved for the interpreter, 0x200-0xFFF is for programs)
#define CHIP8_MEMORY_SIZE 4096 
// 16 8-bit GP registers (V0-VF) (VF is used as a flag)
// There is also a 16-bit I register (index register) which is used to store memory addresses and a 16-bit program counter (PC)
#define CHIP8_REGISTER_COUNT 16 
// 16 levels of stack
#define CHIP8_STACK_SIZE 16 
// chip 8 has a 16-key hexadecimal keypad
#define CHIP8_KEYPAD_SIZE 16 
// Width of the display
#define CHIP8_DISPLAY_WIDTH 64
// Height of the display
#define CHIP8_DISPLAY_HEIGHT 32 
#define CHIP8_DISPLAY_SIZE (CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT) // Total number of pixels in the display
#define CHIP8_START_ADDRESS 0x200 


typedef struct {
    // CPU Registers
    uint8_t V[CHIP8_REGISTER_COUNT]; 
    uint16_t I; // Index register
    uint16_t PC; // Program counter

    uint8_t memory[CHIP8_MEMORY_SIZE]; // Memory

    
    uint16_t stack[CHIP8_STACK_SIZE]; // Stack
    uint8_t SP; // Stack pointer
    
    // Timers
    uint8_t delay_timer;        // Delay timer (decrements at 60Hz)
    uint8_t sound_timer;        // Sound timer (decrements at 60Hz, sounds a beep when it reaches 0)

    // Display
    uint8_t display[CHIP8_DISPLAY_SIZE]; 

    // Keypad State 
    bool keypad[CHIP8_KEYPAD_SIZE];         // (false = not pressed, true = pressed)

    // Current opcode
    uint16_t opcode;

    // State Flags
    bool drawFlag;             // Set to true when the display needs to be updated
} chip8_t;

// Function Prototypes

void initializeCPU(chip8_t *chip8);
void loadROM(chip8_t *chip8, const char *romPath);
void executeCycle(chip8_t *chip8); //(fetch, decode, execute)
uint16_t fetchOpcode(chip8_t *chip8);
void decodeAndExecute(chip8_t *chip8, uint16_t opcode);
void clearDisplay(chip8_t *chip8);
bool drawSprite(chip8_t *chip8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height);
bool isKeyPressed(chip8_t *chip8, uint8_t key);
void clearDisplay(chip8_t *chip8); 
bool isKeyPressed(chip8_t *chip8, uint8_t key);
uint8_t waitForKeyPress(chip8_t *chip8);

/* The function drawSprite XORs each bit of the sprite with the pixel on the display it corresponds to.
    If a pixel is turned off as a result of the XOR operation, the function returns true (e.g. collision), otherwise it returns false.
*/
bool drawSprite(chip8_t *chip8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height); 

#endif // CHIP8_H