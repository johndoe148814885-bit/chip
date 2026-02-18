#ifndef CHIP_MAIN
#define CHIP_MAIN
#define RAM_SIZE 4096
#define RAM_FONT 80
#define RAM_PROGRAM 200
#define RAM_STACK 4032
#define LOAD_DELAY 0.01
#define CYCLE_DELAY 3

extern unsigned char RAM[RAM_SIZE];
extern unsigned char VX[0xF];
extern unsigned short PC, SP, I;
extern unsigned char DT, ST;
extern unsigned char display[8 * 32];

#endif
