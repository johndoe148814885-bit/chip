#include "main.h"
#include "opcode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>

int running = 1;
int updatedisplay = 1;
char* binaryname;
struct winsize win;

// physical memory emulations
unsigned char RAM[RAM_SIZE];
unsigned char VX[16];
unsigned short PC, SP, I;
unsigned char DT, ST;
unsigned char display[8 * 32]; // 64 cols x 32 rows of 2 characters, represented by 8 x 32 bytes, made up of 64 x 32 bits

void sigint(int) {
	running = 0;}

void toggletermbuffer(int alt) {
	if (alt) {
		printf("\033[s"); // save cursor
		printf("\033[?25l"); // hide cursor
        	printf("\033[?1049h"); // alternate buffer
		printf("\033[2J"); // clear buffer
        	printf("\033[H");} // set cursor to top left	
	else {
		printf("\033[2J"); // clear buffer
        	printf("\033[?1049l"); // restore buffer
        	printf("\033[u"); // restore cursor
		printf("\033[?25h");}} // show cursor

int loadfont() {
	toggletermbuffer(1);
	printf("Loading font into RAM");

	unsigned char font[80] = {
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
        	0xF0, 0x80, 0xF0, 0x80, 0x80}; // F
	
	for (int i = 0; (i < RAM_PROGRAM - RAM_FONT) && (i < 80) && running; ++i) {
		RAM[i + RAM_FONT] = font[i];

		for (int ii = 0; ii <= i && ii < win.ws_row - 1; ++ii)
			printf("\033[%d;1H0x%04X: %02X %02X", ii + 2, RAM_FONT + i - ii, RAM[RAM_FONT + i - ii], RAM[RAM_FONT + i - ii + 1]);
		
		fflush(stdout);
		usleep(LOAD_DELAY * CLOCKS_PER_SEC);}
	
	toggletermbuffer(0);
	return !running;}

int loadprogram(char* path) {
	toggletermbuffer(1);
	printf("Loading binary '%s' into RAM", binaryname);	

	FILE* file = fopen(path, "rb");
	if (file == NULL) return 1;
	
	for (int i = 0; (fread(&(RAM[RAM_PROGRAM + i]), 1, 2, file) == 2) && (i < RAM_STACK - RAM_PROGRAM) && running; i += 2) {
		for (int ii = 0; ii <= i && ii < win.ws_row - 1; ++ii)
			printf("\033[%d;1H0x%04X: %02X %02X", ii + 2, RAM_PROGRAM + i - ii, RAM[RAM_PROGRAM + i - ii], RAM[RAM_PROGRAM + i - ii + 1]);
		
		fflush(stdout);		
		usleep(LOAD_DELAY * CLOCKS_PER_SEC);}  
			
	fclose(file);
	toggletermbuffer(0);
	
	return !running;}

int main(int argc, char** argv) {
	// validate argc
	if (argc != 2) {
		printf("Incorrect number of arguments; usage: %s <.ch8 binary>\n", argv[0]);
		return 1;}
	
	// get terminal width and height in characters
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1)
		printf("Incompatible terminal.\n");
	
	signal(SIGINT, sigint); // catch signal interrupt (ctrl+c) and make it quit safely
	
	// load font	
	printf("Loading font into RAM\n");
	if (loadfont() == 1) {
		printf("Failed to load font into RAM\n");
		return 1;};
	printf("Loaded font into RAM\n");
	
	// get everything after the last / in the path to the .ch8 binary
	char *slash = strrchr(argv[1], '/');

	if (slash)
        	binaryname = strdup(slash + 1);
	else
        	binaryname = strdup(argv[1]);

	// load binary
	printf("Loading binary '%s' into RAM\n", binaryname);
	if (loadprogram(argv[1]) == 1) {
		printf("Failed to load binary '%s' into RAM\n", binaryname);
		return 1;}
	printf("Loaded binary '%s' into RAM\n", binaryname);
	
	// begin program execution
	printf("Executing program\n");
	toggletermbuffer(1);
	
	PC = RAM_PROGRAM;
	SP = RAM_STACK;
	
	// print unchanged chars
	printf("Display (%s)", binaryname);
	printf("\033[1;131HRegisters");	
	printf("\033[1;147HStack");
	printf("\033[1;163HProgram");
	
	/*display[0] = 0xAE;  display[1] = 0x88;  display[2] = 0xE0;
	display[8] = 0xA8;  display[9] = 0x88;  display[10] = 0xA0;
	display[16] = 0xEE; display[17] = 0x88; display[18] = 0xA0;
	display[24] = 0xA8; display[25] = 0x88; display[26] = 0xA0;
	display[32] = 0xAE; display[33] = 0xEE; display[34] = 0xE0;
	display[7] = 0x01;  display[248] = 0x80;display[255] = 0x01;
	*/
	char opcodehistory[win.ws_row - 1][64];	int nopcodeentries = 0;
	
	clock_t t = clock();

	while (running == 1 && ((unsigned short)RAM[PC] << 8) + RAM[PC + 1] != 0x0000) {		
		// execute opcode
		char opcodeentry[64]; 
		snprintf(opcodeentry, sizeof(opcodeentry), "0x%04X: %02X %02X \033[31m%s\033[0m\033[0K", PC, RAM[PC], RAM[PC + 1], idtopseudo(opcodetoid(RAM[PC], RAM[PC + 1])));
		
		int index = nopcodeentries++ % (win.ws_row - 1);
		strncpy(opcodehistory[index], opcodeentry, 63);
		opcodehistory[index][63] = '\0';
		
		idtofunc(opcodetoid(RAM[PC], RAM[PC + 1]))();
		
		// print registers
		printf("\033[2;131HPC: 0x%04X", PC);
		printf("\033[3;131HSP: 0x%04X", SP);
		printf("\033[4;131HI: 0x%04X", I);
		printf("\033[5;131HDT: %d", DT);
		printf("\033[6;131HST: %d", ST);
		for (int i = 0; i <= 0xF; ++i)
			printf("\033[%d;131HV%X: %d", i + 7, i, VX[i]);

		// print stack
		for (int i = 0; i < (SP - RAM_STACK) / 2 && i < win.ws_row - 1; ++i)
			printf("\033[%d;147H0x%04X: %02X %02X", i + 2, SP - (i * 2) - 2, RAM[SP - (i * 2) - 2], RAM[SP - (i * 2) - 1]);

		// print program
		int max_history = win.ws_row - 1;
		int count = nopcodeentries < max_history ? nopcodeentries : max_history;

		for (int i = 0; i < count; ++i) {
    			int hist_index = (nopcodeentries - 1 - i) % max_history;
    			if (hist_index < 0)
        			hist_index += max_history;

    			printf("\033[%d;163H%s\033[0K", i + 2, opcodehistory[hist_index]);
		}

		// print display
		if (updatedisplay) {
			updatedisplay = 0;

			for (int i = 0; i < 64 * 32; ++i) {
				int row = i / 64 + 2;
				int col = (i % 64) * 2 + 1;
				int byteindex = i / 8;
				int bitoffset = i % 8;
				int on = (display[byteindex] & (128 >> bitoffset)) != 0;

				if (on)
					printf("\033[%d;%dH\033[47;30m  \033[0m", row, col);
				else
					printf("\033[%d;%dH  ", row, col);}}
		
		fflush(stdout);
		
		if (clock() - t >= CLOCKS_PER_SEC / 60) {
    			if (DT > 0) {
        			DT -= (clock() - t) / (CLOCKS_PER_SEC / 60);
        			if (DT < 0) DT = 0;}

    			if (ST > 0) {
        			ST -= (clock() - t) / (CLOCKS_PER_SEC / 60);
        			if (ST < 0) ST = 0;}

    			t += ((clock() - t) / (CLOCKS_PER_SEC)) * (CLOCKS_PER_SEC / 60);}


		usleep(CYCLE_DELAY * CLOCKS_PER_SEC);}

	toggletermbuffer(0);
	printf("Execution complete\n");

	return 0;}
