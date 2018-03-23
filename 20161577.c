#include "main.h"

int main() {
	char inp[CMD_LEN];
	char tmp[CMD_LEN];
	int i, j;
	USR_CMD cmdExec;

	resetCMD();
	hash_create();

	while(1) {
		printf("sicsim> ");
		fgets(inp, CMD_LEN, stdin);
		inp[strlen(inp) - 1] = '\0';
		
		j = 0;
		for(i = 0; inp[i]; i++) {
			if(inp[i] == ',') {
				strcpy(tmp + j, " , ");
				j += 3;
			}
			else
				tmp[j++] = inp[i];
		}
		tmp[j] = '\0';
		cmdExec = findCMD(tmp);
		if(cmdExec.cmd < invFormat)
			hist_add(inp);

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
