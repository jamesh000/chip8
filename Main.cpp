#include <iostream>
#include "Chip8.h"

int main()
{
    Chip8 c;

    unsigned char A[5] {0xF0, 0x90, 0xF0, 0x90, 0x90};

    unsigned char prog[] = {
	// 0x200
	0x12, 0x10,  // Reserve 16 bytes
	0x00, 0xE0,  // 0 - Clear the screen
	0xC1, 0xFF,  // 2 - Random number in V1
	
	0xF1, 0x33,  // Store
	0xF1, 0x29,  // 4 - Set I to position of hex character in V1
	0xC2, 0x3A,  // 6 - Set V2 to random x coordinate
	0xC3, 0x1B,  // 8 - Set V3 to random y coordinate
	0xD2, 0x35,  // A - Draw hex char in V1 to coordinates in V2 and V3
	0x3F, 0x01,  // C - Skip the jump if there was a collision
	0x12, 0x02,  // E - Jump to beginning
	// 0x210
	0xD2, 0x35,  // 0 - Fix collision
	0x22, 0x16,  // 2 - Waste a second
	0x12, 0x00,  // 4 - Total reset
	// subroutine to wait out the delay timer
	0xFA, 0x07,  // Set FA to the value of the delay timer
	0x3A, 0x00,  // Skip the following instruction if FA is 0
	0x12, 0x1A,  // Restart the loop
	0x00, 0xEE   // Return to caller
    };

    c.store(prog, sizeof(prog), 0x200);

    while (c.cycle())
	;

    return 0;
}
