#ifndef HARDWAREHPP
#define HARDWAREHPP
#include <cstdint>

// Implements a Chip 8 style display
class Display {
public:
    Display();
    
    void clear();
    uint8_t drawsprite(int x, int y, int len, const uint8_t *sprite);
    void updatescreen();
private:
    uint64_t display[32];
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
