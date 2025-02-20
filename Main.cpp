#include <cstdint>
#include <iostream>
#include "Chip8.h"

int main()
{
    Chip8 cpu;

    uint8_t prog2[] = {
        // 0x200
        0x00, 0xE0, // 0 - clear screen
        0xC1, 0x0F, // 2 - random number from 0-F in V1
        0xF1, 0x29, // 4 - get character font in V1
        0xC2, 0x3F, // 6 - random number from 0-63 in V2
        0xC3, 0x1F, // 8 - random number from 0-31 in V3
        0xD2, 0x35, // a - draw character
        0x22, 0x10, // c - time killer subroutine
        0x12, 0x00, // e - restart
        // 0x210
        0x64, 0x3C, // 0 - load 60 into V4
        0xF4, 0x15, // 2 - move VA into delay timer
        0xF4, 0x07, // 4 - store delay timer value in V4
        0x34, 0x00, // 6 - if not zero, loop again
        0x12, 0x14, // 8 - loop
        0x00, 0xEE, // a - otherwise, return
    };

    cpu.store(prog2, sizeof(prog2), 0x200);

    cpu.run();

    return 0;
}
