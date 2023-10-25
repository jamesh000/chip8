#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>
#include "Chip8.h"

#define V0 (V[0x0])
#define VX (V[opc[1]])
#define VY (V[opc[2]])
#define VF (V[0xF])

/*
 *  Implementation of Chip 8 Machine
 */
Chip8::Chip8()
{
    // Clear registers
    for (int i = 0; i < 16; i++) {
	V[i] = 0;
    }

    // Clear stack
    for (int i = 0; i < 16; i++) {
	stack[i] = 0;
    }

    // Start timers
    delaytimer.count = 0;
    dtthread = std::thread(countdown, std::ref(delaytimer), std::ref(runtimers));
    soundtimer.count = 0;
    stthread = std::thread(countdown, std::ref(soundtimer), std::ref(runtimers));

    // Seed the rng
    re.seed(rd());
}

Chip8::~Chip8()
{
    std::cout << "Stopping timers" << std::endl;
    runtimers = false;
    dtthread.join();
    stthread.join();
    std::cout << "Stopped timers" << std::endl;
}


int Chip8::exec(Opcode opc)
{
    unsigned char temp;
    
    switch (opc[0]) {
    case 0x0:
	switch(opc(1,4)) {
	case 0x0E0:
	    screen.clear();
	    draw = true;
	    break;
	case 0x0EE:
	    sp--;
	    pc = *sp;
	    pcinc = false;
	    break;
	default:
	    return -1;
	}
	break;
    case 0x1:
	pc = opc(1,4);
	pcinc = false;
	break;
    case 0x2:
	*sp = pc+2;
	sp++;
	pc = opc(1,4);
	pcinc = false;
	break;
    case 0x3:
	if (VX == opc(2,4))
	    pc += 2;
	break;
    case 0x4:
	if (VX != opc(2,4))
	    pc += 2;
	break;
    case 0x5:
	if (VX == VY)
	    pc += 2;
	break;
    case 0x6:
	VX = opc(2,4);
	break;
    case 0x7:
	VX += opc(2,4);
	break;
    case 0x8:
	switch (opc[3]) {
	case 0x0:
	    VX == VY;
	    break;
	case 0x1:
	    VX |= VY;
	    break;
	case 0x2:
	    VX &= VY;
	    break;
	case 0x3:
	    VX ^= VY;
	    break;
	case 0x4:
	    VX += VY;
	    (VX < VY) ? VF = 0x00 : VF = 0x01;
	    break;
	case 0x5:
	    temp = VX;
	    VX -= VY;
	    (VX > temp) ? VF = 0x00 : VF = 0x01;
	    break;
	case 0x6:
	    VF = VY & 0x01;
	    VX = VY >> 1;
	    break;
	case 0x7:
	    VX = VY - VX;
	    (VX > VY) ? VF = 0x00 : VF = 0x01;
	    break;
	case 0xE:
	    VF = VY & 0x80;
	    VX = VY << 1;
	    break;
	default:
	    return -1;
	}
    case 0x9:
	if (VX != VY)
	    pc += 2;
	break;
    case 0xA:
	I = opc(1,4);
	break;
    case 0xB:
	pc = opc(1,4) + V0;
	pcinc = false;
	break;
    case 0xC:
	VX = dist(re) & opc(2,4);
	break;
    case 0xD:
	VF = screen.drawsprite(VX, VY, &memory[I], opc[3]);
	draw = true;
	break;
    case 0xE:
	// ******To be implemented******
	// The instructions in the 0xE000 range handle keyboard input
	break;
    case 0xF:
	switch (opc(2,4)) {
	case 0x07:
	    delaytimer.mtx.lock();
	    VX = delaytimer.count;
	    delaytimer.mtx.unlock();
	    break;
	case 0x0A:
	    // ******To be implemented******
	    // This instruction handles keyboard input
	    break;
	case 0x15:
	    delaytimer.mtx.lock();
	    delaytimer.count = VX;
	    delaytimer.mtx.unlock();
	    break;
	case 0x18:
	    soundtimer.mtx.lock();
	    soundtimer.count = VX;
	    soundtimer.mtx.unlock();
	    break;
	case 0x1E:
	    I += VX;
	    break;
	case 0x29:
	    (VX <= 0x0F) ? I = 0x50 + 5*VX : I = 0x50;
	    break;
	case 0x33:
	    // Binary coded decimal
	    memory[I] = VX/100;
	    memory[I+1] = (VX%100)/10;
	    memory[I+2] = VX%10;
	    break;
	case 0x55:
	    for (int i = 0; i <= opc[1]; i++)
		memory[I+i] = V[i];
	    I += opc[1] + 1;
	    break;
	case 0x65:
	    for (int i = 0; i <= opc[1]; i++)
		V[i] = memory[I+i];
	    I += opc[1] + 1;
	    break;
	default:
	    return -1;
	}
	break;
    default:
	return -1;
    }
    return 0;
}

// At some point sound timer functionality needs to be added
int Chip8::cycle()
{
    using namespace std::chrono_literals;
    auto cycle_end = std::chrono::system_clock::now() + 1666667ns; //real cycle length
    
    Opcode o = memory.getop(pc);

    if (o(0,4) == 0x0000) {
	std::cout << "0x0000 hit, ending run" << std::endl;
	return 0;
    }
    
    if (exec(o) < 0) {
	std::cout << "Invalid Chip 8 instruction 0x" << o.prettyprint() << " at address " << std::hex << pc << std::endl;
	throw std::exception{};
    }
    
    if (draw) {
	screen.updatescreen();
	draw = false;
    }
    
    if (pcinc)
	pc += 2;
    else
	pcinc = true;
    
    std::this_thread::sleep_until(cycle_end);
    return 1;
}

// To be completed
int Chip8::loadprogram(std::string filename)
{
    return -1;
}

void Chip8::store(unsigned char* bytes, unsigned short bytecount, unsigned short addr)
{
    for (int i = 0; i < bytecount; i++)
	memory[addr+i] = bytes[i];
}


/*
 *  Implementation of Opcode type
 */

Opcode::Opcode()
    : code {0x0}
{}

Opcode::Opcode(unsigned short c)
    : code {c}
{}

unsigned short Opcode::operator[](unsigned short i)
{
    if (i >= 0 && i < 4)
	return (code >> (12 - i*4)) & 0x000F;
    else
	throw std::out_of_range{"Opcode::operator[]"};
}

unsigned short Opcode::operator()(unsigned short h, unsigned short l)
{
    if (h > l || h < 0 || l > 4)
	throw std::out_of_range{"Opcode::operator()"};
    
    unsigned short mask = 0x0;
    for (int i = 0; i < l-h; i++) {
	mask <<= 4;
	mask |= 0b1111;
    }
    
    return (code >> (16 - l*4)) & mask;
}

std::string Opcode::prettyprint()
{
    std::string pretty = "";
    for (int i = 0; i < 4; i++) {
	char c = (code >> (12 - 4*i)) & 0x000F;
	if (c <= 0x9)
	    c += '0';
	else
	    c += 'A' - 0xA;
	pretty += c;
    }
    return pretty;
}
