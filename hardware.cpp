#include "hardware.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

/*
 *  Display Implementation
 */

Display::Display()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL couldn't initialize: " << SDL_GetError() << std::endl;
    }

    // Create the window
    win = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        std::cerr << "Window didn't work: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create the renderer
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "Renderer didn't work: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Clear the screen
    clear();
}

void Display::clear()
{
    for (int y = 0; y < 32; y++)
	    display[y] = 0x00000000;
    
    clearWindow();
    SDL_RenderPresent(renderer);
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
    clearWindow();
    for (int y = 0; y < 32; y++) {
	for (int x = 0; x < 64; x++) {
	    if ((display[y] >> (63-x)) & 1) {
		drawPixel(x, y);
	    }
	}
    }
    
    SDL_RenderPresent(renderer);
}

bool Display::pollEvents()
{
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
        {
            return true;
        }
    }
    return false;
}

void Display::clearWindow()
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}

void Display::drawPixel(int x, int y)
{
    int unitX = winWidth/64, unitY = winHeight/32;
    SDL_Rect pixel = {x * unitX, y * unitY, unitX, unitY};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &pixel);
}

Display::~Display()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
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

// Register file
RegisterFile::RegisterFile()
{
    for (int i = 0; i < 16; i++) {
	registers[i] = 0;
    }
}

void RegisterFile::saveRange(int lastReg,  uint16_t addr, MemoryFile& mem)
{
    for (int i = 0; i < lastReg; i++) {
        mem[addr+i] = registers[i];
    }
}

void RegisterFile::loadRange(int lastReg,  uint16_t addr, MemoryFile& mem)
{
    for (int i = 0; i < lastReg; i++) {
        registers[i] = mem[addr+i];
    }
}
