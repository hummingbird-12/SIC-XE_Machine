#include "main.h"
#include "cmdProc.h"
#include "shell.h"
#include "memory.h"
#include "hash.h"

int main() {
	char inp[CMD_LEN]; // input string
	char tmp[CMD_LEN]; // temporary string to copy input
	int i, j;
	USR_CMD cmdExec;

	resetCMD(); // initialize memory
	hash_create(); // create hash table of opcodes

	while(1) {
		printf("sicsim> ");
		fgets(inp, CMD_LEN, stdin); // get input string
		inp[strlen(inp) - 1] = '\0'; // replace \n with null character
		
		// copy input string to tmp but place one space before and after comma ','
		j = 0;
		for(i = 0; inp[i]; i++) {
			if(inp[i] == ',') {
				strcpy(tmp + j, " , "); // place space around commma ','
				j += 3;
			}
			else
				tmp[j++] = inp[i];
		}
		tmp[j] = '\0';

		cmdExec = findCMD(tmp); // find the command format from input string
		if(cmdExec.cmd < invFormat)
			hist_add(inp); // if command is not invalid add to history

		// call function for each command
		switch(cmdExec.cmd) {
			case help:
				helpCMD();
				break;
			case dir:
				dirCMD();
				break;
			case quit:
				quitCMD();
				break;
			case hist:
				histCMD();
				break;
			case dump:
				dumpCMD(cmdExec);
				break;
			case edit:
				editCMD(cmdExec);
				break;
			case fill:
				fillCMD(cmdExec);
				break;
			case reset:
				resetCMD();
				break;
			case op:
				opCMD(cmdExec);
				break;
			case oplist:
				oplistCMD();
				break;
			case invFormat:
				invFormatCMD();
				break;
			case invHex:
				invHexCMD();
				break;
			case invVal:
				invValCMD();
				break;
		}
	}
	return 0;
}
