#ifndef HARDWAREHPP
#define HARDWAREHPP
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include <cstdint>
#include <SDL2/SDL.h>

// Implements a Chip 8 style display
class Display {
public:
    Display();
    ~Display();
    
    void clear();
    uint8_t drawsprite(int x, int y, int len, const uint8_t *sprite);
    void updatescreen();
    bool pollEvents();
private:
    void clearWindow();
    void drawPixel(int x, int y);
    
    const int winWidth = 1280, winHeight = 640;
    uint64_t display[32];
    SDL_Window* win = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event e;
};

// Implements Chip 8 style memory
class MemoryFile {
public:
    MemoryFile();

    uint8_t& operator[](uint16_t addr);
private:
    uint8_t mem[4096];
};

class RegisterFile {
public:
    RegisterFile();

    uint8_t& operator[](int regIndex) { return registers[regIndex]; }
    uint16_t& I() { return memReg; }
    void saveRange(int lastReg, uint16_t addr, MemoryFile& mem);
    void loadRange(int lastReg, uint16_t addr, MemoryFile& mem);
private:
    uint8_t registers[16];
    uint16_t memReg;
};

#endif
