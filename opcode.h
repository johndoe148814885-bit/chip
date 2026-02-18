#ifndef CHIP_OPCODE
#define CHIP_OPCODE

typedef void (*opcodefunc)();

extern int opcodetoid(unsigned char, unsigned char);
extern const char* idtopseudo(int);
extern opcodefunc idtofunc(int);

#endif
