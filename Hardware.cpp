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
    for (int x = 0; x < 64; x++)
	for (int y = 0; y < 32; y++)
	    display[x][y] = 0x0;
}

unsigned char Display::drawsprite(unsigned short x, unsigned short y, const unsigned char *sprite, unsigned short bytecount)
{
    unsigned char unset = 0x00;
    
    x %= 64;
    y %= 32;
    
    for (int row = 0; row < bytecount && y+row < 32; row++) {
	for (int i = 0; i < 8; i++) {
	    unsigned char pixelstate = (sprite[row] >> 7-i) & 0x1;
	    if (x+i < 64) {
		unsigned char temp = display[x+i][y+row];
		display[x+i][y+row] ^= pixelstate;
		if (temp && !display[x+i][y+row])
		    unset = 0x01;
	    }
	}
    }

    return unset;
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
	    if (display[x][y]) {
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

Memory::Memory()
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

unsigned char& Memory::operator[](unsigned short addr)
{
    if (addr >= 0x0 && addr <= 0xFFF)
	return mem[addr];
    return mem[0x0];
}

Opcode Memory::getop(unsigned short addr)
{
    Opcode fetchedcode {mem[addr] << 8 | mem[addr+1]};
    return fetchedcode;
}


/*
 * Timer Implementation
 */

void countdown(Timer& t, bool& run)
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    system_clock::time_point countend;
    
    while (run) {
	countend = system_clock::now() + 16666667ns;
	
        t.mtx.lock();
	if (t.count > 0)
	    t.count--;
	t.mtx.unlock();
	
	std::this_thread::sleep_until(countend);
    }
}
