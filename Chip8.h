#include <random>
#include <thread>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdint>

constexpr std::chrono::nanoseconds CYCLE_TIME(1666667);

// Implements Chip 8 style memory
class MemoryFile {
public:
    MemoryFile();

    uint8_t& operator[](uint16_t addr);
private:
    uint8_t mem[4096];
};

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

// The Chip 8 interpreter
class Chip8 {
public:
    Chip8();
    //~Chip8();

    int loadprogram(std::string filename);
    void store(uint8_t* bytes, int bytecount, int addr);
    int run();
private:
    // Instruction execution
    int exec(uint16_t opc);

    // Registers
    uint16_t pc = 0x200;
    uint16_t I = 0x000;
    
    // Memory and display
    RegisterFile regFile;
    MemoryFile memFile;
    Display screen;
    
    // Stack starts at 0x200 and grows down
    uint16_t sp = 0x200;
    
    // Delay timer and sound timer
    uint8_t delayTimer;
    uint8_t soundTimer;
    
    // Draw flag and program counter update flag
    bool draw = true;
    bool pcInc = true;
    
    // For random number generation by CXXX opcode
    std::mt19937 re;
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]
};
