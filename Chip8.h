#include <random>
#include <thread>
#include <mutex>

// Wraps a unsigned short with some useful functions
class Opcode {
public:
    Opcode();
    Opcode(unsigned short c);

    unsigned short operator[](unsigned short i);
    unsigned short operator()(unsigned short h, unsigned short l);
    std::string prettyprint();
private:
    unsigned short code;
};


// Implements a Chip 8 style display
class Display {
public:
    Display();

    void clear();
    unsigned char drawsprite(unsigned short x, unsigned short y, const unsigned char *sprite, unsigned short bytecount);
    void updatescreen();
private:
    unsigned char display[64][32];
};


// Implements Chip 8 style memory
class Memory {
public:
    Memory();

    unsigned char& operator[](unsigned short addr);
    Opcode getop(unsigned short address);
private:
    unsigned char mem[4096];
};


// Implements a Chip 8 style timer
struct Timer {
    unsigned char count;
    std::mutex mtx;
};

void countdown(Timer& t, bool& run);


// The Chip 8 interpreter
class Chip8 {
public:
    Chip8();
    ~Chip8();

    int exec(Opcode opc);
    int loadprogram(std::string filename);
    void store(unsigned char* bytes, unsigned short bytecount, unsigned short addr);
    int cycle();
private:
    // Registers
    unsigned char V[16];
    unsigned short pc = 0x200;
    unsigned short I = 0x000;

    // Memory and display
    Memory memory;
    Display screen;

    // Stack
    unsigned short stack[16];
    unsigned short* sp = &stack[0];

    // Delay timer and sound timer
    bool runtimers = true;
    Timer delaytimer;
    std::thread dtthread;
    Timer soundtimer;
    std::thread stthread;

    // Draw flag and program counter update flag
    bool draw = true;
    bool pcinc = true;

    // For random number generation by CXXX opcode
    std::random_device rd;
    std::default_random_engine re;
    std::uniform_int_distribution<int> dist {0x0, 0xFF};
};
