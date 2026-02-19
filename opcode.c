#include "opcode.h"
#include "main.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

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
	updatedisplay = 1;
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
	int X = VX[RAM[PC] & 0x0F];
	int NN = RAM[PC + 1];
	PC += (X == NN) * 2 + 2;}
	
void opcode4XNN() {
	int X = VX[RAM[PC] & 0x0F];
	int NN = RAM[PC + 1];
	PC += (X != NN) * 2 + 2;}

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
	int X = RAM[PC] & 0x0F;
	int NN = RAM[PC + 1];
	VX[X] += NN;
	PC += 2;};

void opcode8XY0() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[X] = Y;
	PC += 2;};

void opcode8XY1() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[X] |= Y;
	PC += 2;};

void opcode8XY2() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[X] &= Y;
	PC += 2;};

void opcode8XY3() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[X] ^= Y;
	PC += 2;};

void opcode8XY4() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[0xF] = VX[X] + Y > 0xFF;
	VX[X] += Y;
	PC += 2;};

void opcode8XY5() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[0xF] = VX[X] >= Y;
	VX[X] -= Y;
	PC += 2;};

void opcode8XY6() {
	int X = RAM[PC] & 0x0F;
	VX[0xF] = VX[X] & 0x01;
	VX[X] >>= 1;
	PC += 2;};

void opcode8XY7() {
	int X = RAM[PC] & 0x0F;
	int Y = VX[RAM[PC + 1] >> 4];
	VX[0xF] = Y >= VX[X];
	VX[X] = Y - VX[X];
	PC += 2;};

void opcode8XYE() {
	int X = RAM[PC] & 0x0F;
	VX[0xF] = (VX[X] & 0x80) != 0;
	VX[X] <<= 1;
	PC += 2;};

void opcode9XY0() {
	int X = VX[RAM[PC] & 0x0F];
	int Y = VX[RAM[PC + 1] >> 4];
	PC += (X != Y) * 2 + 2;};

void opcodeANNN() {
	int NNN = ((RAM[PC] & 0x0F) << 8) + RAM[PC + 1];
	I = NNN;
	PC += 2;};

void opcodeBNNN() {
	int NNN = ((RAM[PC] & 0x0F) << 8) + RAM[PC + 1];
	PC = VX[0] + NNN;};

void opcodeCXNN() {
	int X = RAM[PC] & 0x0F;
	int NN = RAM[PC + 1];
	VX[X] = (rand() % 0x100) & NN;
	PC += 2;};

void opcodeDXYN() {
	int X = VX[RAM[PC] & 0x0F];
	int Y = VX[RAM[PC + 1] >> 4];
	int N = RAM[PC + 1] & 0x0F;
	VX[0xF] = 0;

	for (int i = 0; i < N; ++i) {
		int sprite = RAM[I + i];
		int row = (Y + i) % 32;

		for (int bit = 0; bit < 8; ++bit) {
			int spritebit = (sprite >> (7 - bit)) & 1;
			if (spritebit == 0)
				continue;

			int pixelx = (X + bit) % 64;
			int idx = row * 8 + (pixelx / 8);
			int mask = 1 << (7 - (pixelx % 8));

			if ((display[idx] & mask) != 0)
				VX[0xF] = 1;

			display[idx] ^= mask;}}

	updatedisplay = 1;
	PC += 2;};

void opcodeEX9E() { // not implemented yet
	PC += 2;};

void opcodeEXA1() { // not implemented yet
	PC += 2;};

void opcodeFX07() {
	int X = RAM[PC] & 0x0F;
	VX[X] = DT;
	PC += 2;};

void opcodeFX0A() { // not implemented yet
	PC += 2;};

void opcodeFX15() {
	int X = VX[RAM[PC] & 0x0F];
	DT = X;
	PC += 2;};

void opcodeFX18() {
	int X = VX[RAM[PC] & 0x0F];
	ST = X;
	PC += 2;};

void opcodeFX1E() {
	int X = VX[RAM[PC] & 0x0F];
	I += X;
	PC += 2;};

void opcodeFX29() {
	int X = VX[RAM[PC] & 0x0F];
	I = X * 5 + RAM_FONT;
	PC += 2;};

void opcodeFX33() {
	int X = VX[RAM[PC] & 0x0F];
	RAM[I] = X / 100;
	RAM[I + 1] = (X / 10) % 10;
	RAM[I + 2] = X % 10;
	PC += 2;};

void opcodeFX55() {
	int X = VX[RAM[PC] & 0x0F];
	for (int i = 0; i <= X; ++i)
		RAM[I + i] = VX[i];
	PC += 2;};

void opcodeFX65() {
	int X = VX[RAM[PC] & 0x0F];
	for (int i = 0; i <= X; ++i)
		VX[i] = RAM[I + i];
	PC += 2;};

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
