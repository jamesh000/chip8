#include <cstdint>
#include <iostream>
#include "chip8.hpp"

int main()
{
    Chip8 cpu;

    uint8_t prog2[] = {
        // 0x200
        0x00, 0xE0, // 0 - clear screen
        0xC1, 0x0F, // 2 - random character in V1
        0xF1, 0x29, // 4 - get character font in V1
        0xC2, 0x3F, // 6 - random number from 0-63 in V2
        0xC3, 0x1F, // 8 - random number from 0-31 in V3
        0xD2, 0x35, // a - draw character
        0x22, 0x10, // c - time killer subroutine
        0x12, 0x02, // e - restart
        // 0x210
        0x64, 0x3C, // 0 - load 60 into V4
        0xF4, 0x15, // 2 - move VA into delay timer
        0xF4, 0x07, // 4 - store delay timer value in V4
        0x34, 0x00, // 6 - if not zero, loop again
        0x12, 0x14, // 8 - loop
        0x00, 0xEE, // a - otherwise, return
    };

    uint8_t prog1[] = {
        // 0x200
        0x63, 0x3B, // 0 - set V3 to 59 (x max)
        0x64, 0x1A, // 2 - set V4 to 26 (y max)
        0x6E, 0x00, // 4 - set VE to 0 (exit character)
        0x00, 0xE0, // 6 - clear screen
        0x61, 0x01, // 8 - set V1 to 0 (x pos)
        0x62, 0x01, // a - set V2 to 0 (y pos)
        0xF5, 0x0A, // c - take input, store in V5
        0xF5, 0x29, // e - store location of character font in I
        // 0x210
        0xD1, 0x25, // 0 - draw character
        0xEE, 0xA1, // 2 - reset if character in VE is pressed
        0x12, 0x06, // 4 - reset
        0x71, 0x05, // 6 - increment V1 (x pos) by 6
        0x88, 0x10, // 8 - store value of V1 (x pos) in V8
        0x88, 0x35, // a - subtract x max from x
        0x3F, 0x01, // c - skip next instruction if no borrow (x is greater than x max)
        0x12, 0x0c, // e - skip y increment and loop immediately
        // 0x220
        0x61, 0x01, // 0 - reset V1 (x pos)
        0x72, 0x06, // 2 - increment V2 (y pos)
        0x88, 0x20, // 4 - store value of V2 (y pos) in V8
        0x88, 0x45, // 6 - subtract y max from y pos
        0x3F, 0x01, // 8 - skip next instruction if no borrow (y is greater than y max)
        0x12, 0x0c, // a - skip reset and immediately loop
        0x12, 0x06, // c - restart program
    };

    cpu.store(prog1, sizeof(prog1), 0x200);

    cpu.run();

    return 0;
}
