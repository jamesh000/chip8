#ifndef CHIP8HPP
#define CHIP8HPP
#include <random>
#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <random>
#include <string>
#include "hardware.hpp"

constexpr std::chrono::nanoseconds CYCLE_TIME(1666667);

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

#endif
