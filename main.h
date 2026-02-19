#ifndef CHIP_MAIN
#define CHIP_MAIN
#define RAM_SIZE 4096
#define RAM_FONT 80
#define RAM_PROGRAM 512
#define RAM_STACK 4032
#define LOAD_DELAY 0.003
#define CYCLE_DELAY 0.003

extern unsigned char RAM[RAM_SIZE];
extern unsigned char VX[16];
extern unsigned short PC, SP, I;
extern unsigned char DT, ST;
extern unsigned char display[8 * 32];
extern int updatedisplay;
#endif
