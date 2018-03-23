#include "main.h"

void dumpCMD(USR_CMD uscmd) {
	static int start = 0;
	int end, i, j;

	if(start >= MEM_SIZE)
		start = 0;
	end = start + 159;
	if(end >= MEM_SIZE)
		end = MEM_SIZE - 1;
	if(uscmd.arg_cnt) {
		start = hexToDec(uscmd.arg[0]);
		end = start + 159;
		if(uscmd.arg_cnt == 2)
			end = hexToDec(uscmd.arg[1]);
	}

	for(i = start / 16 * 16; i < (end / 16 + 1) * 16; i++) {
		if(!(i % 16))
			printf("%05X ", i);
		if(i < start || i > end)
			printf("   ");
		else
			printf("%02X ", mem[i]);
		if(!((i + 1) % 16)) {
			printf("; ");
			for(j = i - 15; j <= i; j++)
				printf("%c", ((j >= start && j <= end) && mem[j] >= 32 && mem[j] <= 126) ? mem[j] : '.');
			puts("");
		}
	}
	start = end + 1;
}

void editCMD(USR_CMD uscmd) {
	int add, val;
	add = hexToDec(uscmd.arg[0]);
	val = hexToDec(uscmd.arg[1]);
	mem[add] = val;
}

void fillCMD(USR_CMD uscmd) {
	int i, start, end, val;
	start = hexToDec(uscmd.arg[0]);
	end = hexToDec(uscmd.arg[1]);
	val = hexToDec(uscmd.arg[2]);
	for(i = start; i <= end; i++)
		mem[i] = val;
}

void resetCMD() {
	int i, j = 0;
	for(i = 0; i < MEM_SIZE; i++) {
		mem[i] = j++;
		if(j == 128)
			j = 0;
	}
}
