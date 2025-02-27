#include "chip8.hpp"

/*
 *  Implementation of Chip 8 Machine
 */
Chip8::Chip8()
    : re(std::random_device{}()), dist(0x00, 0xFF)
{
    // Set up fontset
    uint8_t font[] =
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
    
    for (int i = 0; i < 16 * 5; i++) {
        memFile[i] = font[i];
    }
    
    // Start timers
    delayTimer = 0;
    soundTimer = 0;
}

/*
Chip8::~Chip8()
{
    
}
*/

int Chip8::exec(uint16_t opc)
{
    unsigned char temp;

    int X = (opc & 0x0F00) >> 8, Y = (opc & 0x00F0) >> 4;
    uint8_t &V0 = regFile[0x0], &VX = regFile[X], &VY = regFile[Y], &VF = regFile[0xF];
    uint16_t& I = regFile.I();
    uint8_t N = opc & 0x000F;
    uint8_t NN = opc & 0x00FF;
    uint16_t NNN = opc & 0x0FFF;
    uint8_t key;
    
    switch (opc & 0xF000) {
    case 0x0000:
        switch (opc & 0x0FFF) {
        case 0x00E0:
            // 00E0 - Clear screen
            screen.clear();
            draw = true;
            break;
        case 0x00EE:
            // 00EE - Return from a subroutine
            pc = memFile[sp] << 8 | memFile[sp+1];
            sp += 2;
            break;
        case 0x0000:
            // 0NNN - execute machine language subroutine at address NNN
            // not feasible to implement on modern hardware
            return 2;
            break;
        default:
            return 1;
        }
        break;
    case 0x1000:
        // 1NNN - jump to address NNN
        pc = NNN;
        pcInc = false;
        break;
    case 0x2000:
        // 2NNN - execute subroutine at NNN
        sp -= 2;
        memFile[sp] = pc >> 8;
        memFile[sp+1] = pc & 0x00FF;
	pc = NNN;
        pcInc = false;
        break;
    case 0x3000:
        // 3XNN - skip next instruction if VX = NN
        if (VX == NN)
            pc += 2;
        break;
    case 0x4000:
        // 4XNN - skip next instruction if VX != NN
        if (VX != NN)
            pc += 2;
        break;
    case 0x5000:
        // 5XY0 - skip next instruction if VX = VY
        if (VX == VY)
            pc += 2;
        break;
    case 0x6000:
        // 6XNN - store number NN in VX
        VX = NN;
        break;
    case 0x7000:
        // 7XNN - add value NN to VX
        VX += NN;
        break;
    case 0x8000:
        switch (opc & 0x000F) {
        case 0x0000:
            // 8XY0 - store value of VY in VX
            VX = VY;
            break;
        case 0x0001:
            // 8XY1 - set VX to VX or VY
            VX |= VY;
            break;
        case 0x0002:
            // 8XY2 - set VX to VX and VY
            VX &= VY;
            break;
        case 0x0003:
            // 8XY3 - set VX to VX xor VY
            VX ^= VY;
            break;
        case 0x0004:
            // 8XY4 - set VX to VX + VY
            VX += VY;
            VF = (VX < VY) ? 1 : 0;
            break;
        case 0x0005:
            // 8XY5 - set VX to VX - VY
            VF = (VX < VY) ? 0 : 1;
            VX -= VY;
            break;
        case 0x0006:
            // 8XY6 - set VX to VY >> 1    
            VF = (VY << 15) >> 15;
            VX >>= VY;
            break;
        case 0x0007:
            // 8XY7 - set VX to VY - VX
            VF = (VX > VY) ? 0 : 1;
            VX = VY - VX;
            break;
        case 0x000E:
            // 8XYE - set VX to VY << 1
            VF = VY >> 15;
            VX <<= VY;
            break;
        default:
            return 1;
        }
        break;
    case 0x9000:
        // 9XY0 - skip next instruction if VX != VY
        if (VX != VY)
            pc += 2;
        break;
    case 0xA000:
        // ANNN - Store memory address NNN in I
        I = NNN;
        break;
    case 0xB000:
        // BNNN - jump to address NNN + V0
        pc = NNN + V0;
        pcInc = false;
        break;
    case 0xC000:
        // CXNN - generate a random number with mask NN
        VX = dist(re) & NN;
        break;
    case 0xD000:
        // DXYN - Draw sprite of length N at coordinates (VX, VY)
        VF = screen.drawSprite(VX, VY, N, &memFile[I]);
        draw = true;
        break;
    case 0xE000:
        switch (opc & 0x00FF) {
        case 0x009E:
            // EX9E - skip next instruction if key in VX is pressed
            if (screen.keyPressed(VX))
                pc += 2;
            break;
        case 0x00A1:
            // EXA1 - skip next instruction if key in VX is not pressed
            // to be implemented
            if (!screen.keyPressed(VX))
                pc += 2;
            break;
        default:
            return 1;
        }
        break;
    case 0xF000:
        switch (opc & 0x00FF) {
        case 0x0007:
            // FX07 - Store the current value of the delay timer in VX
            VX = delayTimer;
            break;
        case 0x000A:
            // FX0A - wait for a keypress and store in register VX
            key = screen.waitKey();
            if (key == 0xFF) // if exit during wait
                return 3;
            VX = key;
            break;
        case 0x0015:
            // FX15 - set the delay timer to VX
            delayTimer = VX;
            break;
        case 0x0018:
            // FX18 - Set the sound timer to VX
            soundTimer = VX;
            break;
        case 0x001E:
            // FX1E - add VX to I
            I += VX;
            break;
        case 0x0029:
            // FX29 - set I to the memory address of the sprite of the character in VX
            I = VX * 5;
            break;
        case 0x0033:
            // FX33 - store the BCD of VX in I, I+1, I+2
            memFile[I] = VX / 100;
            memFile[I+1] = (VX % 100) / 10;
            memFile[I+2] = VX % 10;
            break;
        case 0x0055:
            // FX55 - store the values of V0 to VX starting at I
            regFile.saveRange(X, I, memFile);
            break;
        case 0x0065:
            // FX65 load from I to I+X+1 into registers V0 to VX
            regFile.loadRange(X, I, memFile);
            break;
        default:
            return 1;
        }
        break;
    default:
        return 1;
    }

    return -1;
}

// At some point sound timer functionality needs to be added
int Chip8::run()
{
    auto nextCycleTime = std::chrono::high_resolution_clock::now();
    int tenCycle = 10;

    while (true) {
        // busy wait loop
        while (std::chrono::high_resolution_clock::now() < nextCycleTime)
            ;
        nextCycleTime += CYCLE_TIME;

        /*
        for (int i = 0; i < 16; i++) {
            std::cout << "V" << i << ": " << std::hex << (int)regFile[i] << std::endl;
        }
        */
        
        // Fetch instruction
        uint16_t opc = (((uint16_t)memFile[pc]) << 8) | memFile[pc+1];

        // Should be removed at some point
        if (opc == 0x0000) {
            std::cout << "0x0000 hit at address " << std::hex << pc << ", ending run" << std::endl;
            return 0;
        }

        int result = exec(opc);
        if (result == 1) {
            std::cout << "Invalid Chip 8 instruction 0x" << std::hex << opc << " at address " << std::hex << pc << std::endl;
            throw std::exception{};
        } else if (result == 2) {
            std::cout << "Chip 8 instruction 0x" << std::hex << opc << " at address " << std::hex << pc << " not yet implemented" << std::endl;
            throw std::exception{};
        } else if (result == 3) {
            return 0;
        }


        tenCycle--;
        if (tenCycle == 0) {
            // Draw if anything to draw
            if (draw) {
                screen.updateScreen();
                draw = false;
            }
            // Decrement Timers
            if (delayTimer > 0)
                delayTimer--;
            if (soundTimer > 0)
                soundTimer--;
            // reset
            tenCycle = 10;
        }

        if (screen.pollEvents()) {
            std::cout << "terminating..." << std::endl;
            exit(1);
        }
        
        // increment 
        if (pcInc)
            pc += 2;
        else
            pcInc = true;
    }
    return 1;
}

// To be completed
int Chip8::loadprogram(std::string filename)
{
    return -1;
}

void Chip8::store(uint8_t* bytes, int bytecount, int addr)
{
    for (int i = 0; i < bytecount; i++)
	memFile[addr+i] = bytes[i];
}
