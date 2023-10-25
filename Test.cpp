#include <iostream>
#include "Chip8.h"

int main()
{
    // This commented out section is a test for the class Display
/*    Memory memtest;
    memtest.wipe();
    memtest.initfont();

    Display screen;
    screen.clear();

    unsigned char L[5] {0x80, 0x80, 0x80, 0x80, 0xF8};
    unsigned char M[5] {0x88, 0b11011000, 0xA8, 0x88, 0x88};
    const unsigned char thing = 0b10001000;
    const unsigned char otherthing = 0b01110000;
    unsigned char O[5] {0b01110000, 0b10001000, thing, thing, otherthing};
    screen.drawsprite(1, 1, L, 5);
    screen.drawsprite(7, 1, M, 5);
    screen.drawsprite(13, 1, &memtest[0x50+(5*0xA)], 5);
    screen.drawsprite(18, 1, O, 5);

    screen.updatescreen();
*/

    // This commented out section is a test for the class Memory
/*    memtest[0x200] = 0xAB;
    memtest[0x201] = 0xCD;

    std::cout << std::hex << memtest.getop(0x200) << std::endl;

    for (int i = 0x50; i <= 0x9F; i++) {
	std::cout << std::hex << (short)memtest[i] << " ";
	if ((i+1) % 5 == 0)
	    std::cout << std::endl;
    }
    */

    // Test for class Opcode
    Opcode op {0x00E0};

    std::cout << std::hex << op(0,4) << std::endl;

    return 0;
}
