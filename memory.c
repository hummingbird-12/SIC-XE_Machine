#include "main.h"
#include "memory.h"

void dumpCMD(USR_CMD uscmd) {
	static int start = 0;
	int end, i, j;

	if(start >= MEM_SIZE)
		start = 0; // if start address exceeded memory limit, reset to 0x00000
	end = start + 159; // set initial end value
	
	if(uscmd.arg_cnt) { // if there was argument entered in command
		start = hexToDec(uscmd.arg[0]);
		end = start + 159;
		if(uscmd.arg_cnt == 2) // if there was 2 arguments
			end = hexToDec(uscmd.arg[1]);
	}
	if(end >= MEM_SIZE)
		end = MEM_SIZE - 1; // if end address exceed memory limit, set to 0xFFFFF


	// start loop from the beginning of each 16 Bytes
	// finish loop till end of each 16 Bytes
	for(i = start / 16 * 16; i < (end / 16 + 1) * 16; i++) {
		if(!(i % 16))
			printf("%05X ", i); // print address for beginning of each line
		if(i < start || i > end)
			printf("   "); // do NOT dump memory outside print range
		else
			printf("%02X ", mem[i]); // dump memory in hexadecimal value
		if(!((i + 1) % 16)) { // line finished
			printf("; ");
			// print content converted into ASCII character
			for(j = i - 15; j <= i; j++)
				printf("%c", ((j >= start && j <= end) && mem[j] >= 32 && mem[j] <= 126) ? mem[j] : '.');
			puts("");
		}
	}
	start = end + 1; // remember last print address
}

void editCMD(USR_CMD uscmd) {
	int add, val;
	add = hexToDec(uscmd.arg[0]); // address to edit
	val = hexToDec(uscmd.arg[1]); // replace value
	mem[add] = val;
}

void fillCMD(USR_CMD uscmd) {
	int i, start, end, val;
	start = hexToDec(uscmd.arg[0]); // fill start address
	end = hexToDec(uscmd.arg[1]);   // fill end address
	val = hexToDec(uscmd.arg[2]);   // fill value
	for(i = start; i <= end; i++)
		mem[i] = val;
}

void resetCMD() {
	int i;
	for(i = 0; i < MEM_SIZE; i++)
		mem[i] = 0; // set memory to 0x00000
}
