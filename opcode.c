#include "opcode.h"
#include "main.h"
#include <stdio.h>

const char* pseudos[] = {"",
        "clear display",
        "return",
        "goto NNN",
        "call NNN",
        "skip if VX == NN",
        "skip if VX != NN",
        "skip if VX == VY",
        "VX = NN",
        "VX += NN",
        "VX = VY",
        "VX |= VY",
        "VX &= VY",
        "VX ^= VY",
        "VX += VY",
        "VX -= VY",
        "VX >>= 1",
        "VX = VY - VX",
        "VX <<= 1",
        "skip if VX != VY",
        "I = NNN",
        "goto NNN + V0",
        "VX = rand & NN",
        "draw VX VY N",
        "skip if VX == key",
        "skip if VX != key",
        "VX = DT",
        "VX = wait for key",
        "DT = VX",
        "ST = VX",
        "I += VX",
        "I = character VX",
        "load VX decimal into I",
        "load I into V0 to VX",
        "load V0 to VX into I"};

void opcode00E0() {
	for (int i = 0; i < 8 * 32; ++i)
		display[i] = 0x00;
	PC += 2;}

void opcode00EE() {
	SP -= 2;
	PC = (RAM[SP] << 8) + RAM[SP + 1];}

void opcode1NNN() {
	int NNN = ((RAM[PC] & 0x0F) << 8) + RAM[PC + 1];
	PC = NNN;}

void opcode2NNN() {
	int NNN = ((RAM[PC] & 0x0F) << 8) + RAM[PC + 1];
	PC += 2;
	RAM[SP] = PC >> 8; RAM[SP + 1] = PC;
	SP += 2;
	PC = NNN;}

void opcode3XNN() {
	int X = RAM[PC] & 0x0F;
	int NN = RAM[PC + 1];
	PC += (VX[X] == NN) * 2 + 2;}
	
void opcode4XNN() {
	int X = RAM[PC] & 0x0F;
	int NN = RAM[PC + 1];
	PC += (VX[X] != NN) * 2 + 2;}

void opcode5XY0() {
	int X = VX[RAM[PC] & 0x0F];
	int Y = VX[RAM[PC + 1] >> 4];
	PC += (X == Y) * 2 + 2;}

void opcode6XNN() {
	int X = RAM[PC] & 0x0F;
	int NN = RAM[PC + 1];
	VX[X] = NN;
	PC += 2;}

void opcode7XNN() {
	};
void opcode8XY0() {
	};
void opcode8XY1() {
	};
void opcode8XY2() {
	};
void opcode8XY3() {
	};
void opcode8XY4() {
	};
void opcode8XY5() {
	};
void opcode8XY6() {
	};
void opcode8XY7() {
	};
void opcode8XYE() {
	};
void opcode9XY0() {
	};
void opcodeANNN() {
	};
void opcodeBNNN() {
	};
void opcodeCXNN() {
	};
void opcodeDXYN() {
	};
void opcodeEX9E() {
	};
void opcodeEXA1() {
	};
void opcodeFX07() {
	};
void opcodeFX0A() {
	};
void opcodeFX15() {
	};
void opcodeFX18() {
	};
void opcodeFX1E() {
	};
void opcodeFX29() {
	};
void opcodeFX33() {
	};
void opcodeFX55() {
	};
void opcodeFX65() {
	};

opcodefunc idtofunc(int id) {
        switch (id) {
        case (1):
                return opcode00E0;
        case (2):
                return opcode00EE;
        case (3):
                return opcode1NNN;
        case (4):
                return opcode2NNN;
        case (5):
                return opcode3XNN;
        case (6):
                return opcode4XNN;
        case (7):
                return opcode5XY0;
        case (8):
                return opcode6XNN;
        case (9):
                return opcode7XNN;
        case (10):
                return opcode8XY0;
        case (11):
                return opcode8XY1;
        case (12):
                return opcode8XY2;
        case (13):
                return opcode8XY3;
        case (14):
                return opcode8XY4;
        case (15):
                return opcode8XY5;
        case (16):
                return opcode8XY6;
        case (17):
                return opcode8XY7;
        case (18):
                return opcode8XYE;
        case (19):
                return opcode9XY0;
        case (20):
                return opcodeANNN;
        case (21):
                return opcodeBNNN;
        case (22):
                return opcodeCXNN;
        case (23):
                return opcodeDXYN;
        case (24):
                return opcodeEX9E;
        case (25):
                return opcodeEXA1;
        case (26):
                return opcodeFX07;
        case (27):
                return opcodeFX0A;
        case (28):
                return opcodeFX15;
        case (29):
                return opcodeFX18;
        case (30):
                return opcodeFX1E;
        case (31):
                return opcodeFX29;
        case (32):
                return opcodeFX33;
        case (33):
                return opcodeFX55;
        case (34):
                return opcodeFX65;
        default:
                return NULL;}}

int opcodetoid(unsigned char b0, unsigned char b1) {
        unsigned short word = (b0 << 8) + b1;

        switch (word & 0xF000) {
        case (0x0000): {
                switch (word & 0x00FF) {
                case (0x00E0): {
                        return 1;}
                case (0x00EE): {
                        return 2;}}
                break;}
        case (0x1000): {
                return 3;}
        case (0x2000): {
                return 4;}
        case (0x3000): {
                return 5;}
        case (0x4000): {
                return 6;}
        case (0x5000): {
                return 7;}
        case (0x6000): {
                return 8;}
        case (0x7000): {
                return 9;}
        case (0x8000): {
                switch (word & 0x000F) {
                case (0x0000): {
                        return 10;}
                case (0x0001): {
                        return 11;}
                case (0x0002): {
                        return 12;}
                case (0x0003): {
                        return 13;}
                case (0x0004): {
                        return 14;}
                case (0x0005): {
                        return 15;}
                case (0x0006): {
                        return 16;}
                case (0x0007): {
                        return 17;}
                case (0x000E): {
                        return 18;}}
                break;}
        case (0x9000): {
                return 19;}
        case (0xA000): {
                return 20;}
        case (0xB000): {
                return 21;}
        case (0xC000): {
                return 22;}
        case (0xD000): {
                return 23;}
        case (0xE000): {
                switch (word & 0x00FF) {
                case (0x009E): {
                        return 24;}
                case (0x00A1): {
                        return 25;}}
                break;}
        case (0xF000): {
                switch (word & 0x00FF) {
                case (0x0007): {
                        return 26;}
                case (0x000A): {
                        return 27;}
                case (0x0015): {
                        return 28;}
                case (0x0018): {
                        return 29;}
                case (0x001E): {
                        return 30;}
                case (0x0029): {
                        return 31;}
                case (0x0033): {
                        return 32;}
                case (0x0055): {
                        return 33;}
                case (0x0065): {
                        return 34;}}
                break;}}
        return 0;}

const char* idtopseudo(int id) {
	return pseudos[id];}
