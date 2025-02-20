#include <cstdint>
#include <iostream>
#include <chrono>
#include "Chip8.h"

/*
 *  Display Implementation
 */

Display::Display()
{
    clear();
}

void Display::clear()
{
    for (int y = 0; y < 32; y++)
	    display[y] = 0x0000000;
}

uint8_t Display::drawsprite(int x, int y, int len, const uint8_t *sprite)
{
    bool overwrite = false;
    x %= 64;
    y %= 32;
    
    for (int row = 0; row < len; row++) {
        uint64_t zeroPos = ((uint64_t)sprite[row]) << 56;
        uint64_t spriteRow = (zeroPos >> x) | (zeroPos << (64 - x));
        display[(y+row) % 32] ^= spriteRow;
        if ((display[(y+row) % 32] & spriteRow) != spriteRow)
            overwrite = true;
    }

    return overwrite ? 0x01 : 0x00;
}

void Display::updatescreen()
{
    std::cout << ' ';
    for (int i = 0; i < 64; i++)
	std::cout << '-';
    std::cout << std::endl;
    
    for (int y = 0; y < 32; y++) {
	std::cout << '|';
	for (int x = 0; x < 64; x++) {
	    if ((display[y] >> (63-x)) & 1) {
		std::cout << '#';
	    } else {
		std::cout << ' ';
	    }
	}
	std::cout << '|' << std::endl;
    }
    
    std::cout << ' ';
    for (int i = 0; i < 64; i++)
	std::cout << '-';
    std::cout << std::endl;
    
    std::cout << std::endl;
}


/*
 *  Memory Implementation
 */

MemoryFile::MemoryFile()
{
    for (int i = 0; i < 4096; i++) {
	mem[i] = 0;
    }
    
    unsigned char fontset[80] =
	{
	    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	    0x20, 0x60, 0x20, 0x20, 0x70, // 1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
    
    for (int i = 0; i < 80; i++) {
	mem[0x50 + i] = fontset[i];
    }
}

unsigned char& MemoryFile::operator[](unsigned short addr)
{
    if (addr >= 0x0 && addr <= 0xFFF)
	return mem[addr];
    return mem[0x0];
}
